/*
Project Goal: Inventory for a kitchen. 

Features:
Make inventory/pantry from scratch
Compare quantity on hand against quantity in inventory
Create Grocery List when quantities drop below minimum
Edit item function allows searching single item and
    a) editing quantity
    b) editing cost
Created a "Receive Groceries" Function that:
    a) steps through grocery list or receipt
    b) "receives" items purchased
    c) confirms cost from receipt

Yet to implement:

Create a "Recipe" function that:
    a) allows types of recipes to be stored
    b) removes select quantities from inventory based on recipe
    c) determines if ingredients are on hand to cook a recipe
    d) automatically updates grocery list and inventory

Bug Fixes: 
    a) incorrect Estimated totals printing on grocery list
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <limits>
#include <ctime>
#include <chrono>
#include "Provision.h"
using namespace std;

// This is the minimum threshold before an item is moved
// To the grocery list. 
const int MINIMUM_INVENTORY = 2;

//Function prototypes
void fillInventory();
Provision* loadInventory(int&);
int binCounter();
void groceryList(int, Provision*);
bool inList(int, Provision*, std::string);
void editQuantity(int, Provision*);
void editPrice(int, Provision*);
void receiveGroceries(int&, Provision*&);


int main()
{
    int totalBins = binCounter();
    Provision* pantry = loadInventory(totalBins);

    int choice;
    std::string grocery;   

    // Menu options; added as functions are added.
    cout << "*****Inventory Management System********\n";
    cout << "________________________________________\n";
    cout << "*****Enter 1 to make inventory**********\n";
    cout << "*****Enter 2 to make grocery list*******\n";
    cout << "*****Enter 3 to search for an item******\n";
    cout << "*****Enter 4 to adjust actual quantity**\n";
    cout << "*****Enter 5 to adjust actual price*****\n";
    cout << "*****Enter 6 to receive grocery order***\n";
    cin >> choice;

    switch(choice)
    {
        case 1: "Making New Inventory:\n";
            fillInventory();
            break;
        case 2: "Completing Grocery List:\n";
            groceryList(totalBins, pantry);
            break;
        case 3: "Search for item\n"; 
            cout << "Item Name: ";
                cin.ignore();
                getline(cin, grocery);
            if (inList(totalBins, pantry, grocery) == true)
                cout << "Item in stock:\n";
            else
                cout << "Item Not Found\n";
            break;
        case 4: "Adjusting Quantity on Hand:\n";
            editQuantity(totalBins, pantry);
            groceryList(totalBins, pantry);
            break;
        case 5: "Adjusting Unit Cost:\n";
            editPrice(totalBins, pantry);
            break;
        case 6: "Receive Grocery Order:\n";
            receiveGroceries(totalBins, pantry);
            groceryList(totalBins, pantry);
            break;
        default:
            cout << "Invalid Selection!\n";
    }

    delete [] pantry;
    pantry = nullptr;
    return 0;
}


/*
-----------------------------------------------------------------------------
From a blank inventory: Asks generally, how many bins to fill then
takes a single item name, the quantity on hand for that item, and the cost 
per unit. It then creates a .csv file and writes the contents of the array to 
the file.
----------------------------------------------------------------------------- 
*/
void fillInventory() 
{
    int totalBins; // How many Provision objects are held in inventory
    cout << "How many bins are in the pantry? ";
    cin >> totalBins;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flush buffer
    std::string tempName;
    int tempQuant;
    double tempCost;

    std::ofstream outFile("pantry.csv");
    
    if (!outFile) 
    {
        std::cerr << "Error creating file!\n";
        return;
    }

    Provision* pantry = new Provision[totalBins];

    // Steps through user inputs for item, quantity, and cost
    for (int count = 0; count < totalBins; count++)
    {
        cout << "\nItem name: ";
        cin.ignore();
        getline(cin, tempName);
        pantry[count].setName(tempName);
        cout << "\nQuantity on Hand: ";
        cin >> tempQuant;
        pantry[count].setQuant(tempQuant);
        cout << "\nItem cost: ";
        cin >> tempCost;
        pantry[count].setCost(tempCost);
    }

    // Saves dynamic array to a .csv file for permanence
    for (int count = 0; count < totalBins; count++)
    {
        outFile << pantry[count].getName() << ","
        << pantry[count].getQuant() << ","
        << pantry[count].getCost() << "\n";
    }



    delete [] pantry;
    pantry = nullptr;
    outFile.close();

    cout << "Pantry file has been saved!\n";
}


/*
-----------------------------------------------------------------------------
Takes input file and loads contents into a dynamically allocated array for 
use in other functions. Will only be called from main() which has the delete[] 
in use to keep things memory safe. 
-----------------------------------------------------------------------------
*/
Provision* loadInventory(int& totalBins)
{   
    totalBins = binCounter();
    std::string line;

    // Creates the file if it doesn't exist, replaces it if it does.
    std::ifstream inFile("pantry.csv");

    if (!inFile)
    {
        std::cerr << "Error opening pantry.csv!\n";
        return nullptr;
    }

    inFile.clear();                 // clear EOF flag
    inFile.seekg(0);                // rewind to beginning

    Provision* pantry = new Provision[totalBins];

    // I had an easier time going into tempVariables than straight into the object
    std::string tempName;
    int tempQuant;
    double tempCost;

    for (int count = 0; count < totalBins; count++)
    {
        getline(inFile, tempName, ',');
        inFile >> tempQuant;
        inFile.ignore(); // skip comma
        inFile >> tempCost;
        inFile.ignore(); // skip newline

        pantry[count].setName(tempName);
        pantry[count].setQuant(tempQuant);
        pantry[count].setCost(tempCost);
    }

    inFile.close();
    return pantry; 
}

/*
-----------------------------------------------------------------------------
Sets the value for total number of bins from the number of line items in the 
.csv file that's being used to allow data to persist between operations. 
-----------------------------------------------------------------------------
*/

int binCounter()
{
    int totalBins = 0 ; // How many Provision objects are held in inventory
    
    std::string line;

    std::ifstream inFile("pantry.csv");

    if (!inFile) 
    {
        std::cerr << "Error reading file!\n";
    }

    // Determines how long the dynamic array needs to be

    while (std::getline(inFile, line)) 
    {
        if (!line.empty())
            ++totalBins;
    }
    return totalBins;
}

/*
-----------------------------------------------------------------------------
Compares the inventory on hand versus a minimum value then creates a list in 
a .txt file from the items on hand that are below the minimum threshold
----------------------------------------------------------------------------- 
*/
void groceryList(int totalBins, Provision* pantryList)
{
    std::ofstream outFile("grocery_list.txt");

    // Gets current time for grocery list print out
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    if (!outFile) 
    {
        std::cerr << "Error creating file!\n";
    }
    
    Provision* shoppingList = new Provision[totalBins]; 

    int onHand, shoppingCount = 0; 

    for (int count = 0; count < totalBins; count++)
    {
        onHand = pantryList[count].getQuant();

        
        if (onHand < MINIMUM_INVENTORY)
        {
            shoppingList[shoppingCount].setName(pantryList[count].getName());
            shoppingList[shoppingCount].setQuant(pantryList[count].getQuant());
            shoppingList[shoppingCount].setCost(pantryList[count].getCost());
            shoppingCount++;
        }

    }

    // Calculates the estimated total from previously recorded costs for items in inventory.
    double estimatedCost;
    for (int count = 0; count < shoppingCount; count++)
    {
        estimatedCost += shoppingList[count].getCost();
    }

    // Formatting for the .txt file for easier readability
    outFile << "**********Grocery List***********\n";
    outFile << std::put_time(std::localtime(&now_time), "%A, %B %d, %Y %I:%M %p") << "\n";
    outFile << "*********************************\n\n";
    for (int count = 0; count < shoppingCount; count++)
    {
        outFile << shoppingList[count].getName() << "\n";
    }
    outFile << "\n";
    outFile << "*********************************\n";
    outFile << "Estimated Price for Groceries: \n" << setw(12) << right
        << setprecision(2) << showpoint << fixed << "$" <<(estimatedCost * MINIMUM_INVENTORY) << endl;

    cout << "Grocery List Created!" << endl;

    delete[] shoppingList;
    shoppingList = nullptr;
    outFile.close();
}

/*
-----------------------------------------------------------------------------
Allows the user to type in the name of an item, then searchs (linearly) the
array for the item name that matches selection and returns a true or false 
value to indicate that it is in stock and there are more than 0 units on hand
-----------------------------------------------------------------------------
*/

bool inList(int totalBins, Provision* pantryList, std::string item)
{
    std::string grocery;
    
    bool inStock = false;

    grocery = item;

    for (int count = 0; count < totalBins; count++)
    {
        if(grocery == pantryList[count].getName() && pantryList[count].getQuant() > 0)
        {
            inStock = true;
        }
    }

    return inStock;
}

/*
-----------------------------------------------------------------------------
Allows editing of quantity on hand then reruns groceryList() function to
catch any items that dropped below the minimum and add them to the list

Currently, allows editing of a single item at a time. Must run multiple
time in order to receive a grocery order or do an inventory.
-----------------------------------------------------------------------------
*/
void editQuantity(int totalBins, Provision* pantryList)
{
    std::string grocery;
    int actualQuantity;
    std::ofstream outFile("pantry.csv");

    cout << "Item Name: ";
    cin.ignore();
    getline(cin, grocery);

    if (inList(totalBins, pantryList, grocery) != true)
    {    
        cout << "Item not in Inventory\n";
        return;
    }  

    cout << "Actual Quantity on Hand: ";
    cin >> actualQuantity;

    if (inList(totalBins, pantryList, grocery) != true)
        cout << "Item not in Inventory\n";

    // Checks quantity on hand for quantity in object array
    for (int count = 0; count < totalBins; count++)
    {
        if (grocery == pantryList[count].getName() && pantryList[count].getQuant() != actualQuantity)
        {    
            pantryList[count].setQuant(actualQuantity);
            cout << "Quantity changed successfully\n";
        }
        else if (grocery == pantryList[count].getName() && pantryList[count].getQuant() == actualQuantity)
            cout << "Quantity in system matches actual quantity on hand\n";
    }
    

    // Saves dynamic array to a .csv file for permanence
    for (int count = 0; count < totalBins; count++)
    {
        outFile << pantryList[count].getName() << ","
        << pantryList[count].getQuant() << ","
        << pantryList[count].getCost() << "\n";
    }
}

/*
-----------------------------------------------------------------------------
Allows editing price for a single line item. Will search list to confirm that
item is actually in the inventory and allow the user to input the new price 
if the item is present in inventory.
-----------------------------------------------------------------------------
*/
void editPrice(int totalBins, Provision* pantryList)
{
    std::string grocery;
    float actualPrice;
    std::ofstream outFile("pantry.csv");


    cout << "Item Name: ";
    cin.ignore();
    getline(cin, grocery);

    if (inList(totalBins, pantryList, grocery) != true)
    {    
        cout << "Item not in Inventory\n";
        return;
    }  

    cout << "Actual Cost of Item: ";
    cin >> actualPrice;

    // Checks quantity on hand for quantity in object array
    for (int count = 0; count < totalBins; count++)
    {
        if (grocery == pantryList[count].getName() && pantryList[count].getCost() != actualPrice)
        {    
            pantryList[count].setCost(actualPrice);
            cout << "Cost changed successfully\n";
        }
        else if (grocery == pantryList[count].getName() && pantryList[count].getCost() == actualPrice)
            cout << "Cost in system matches actual quantity on hand\n";
        
    }

    // Saves dynamic array to a .csv file for permanence
    for (int count = 0; count < totalBins; count++)
    {
        outFile << pantryList[count].getName() << ","
        << pantryList[count].getQuant() << ","
        << pantryList[count].getCost() << "\n";
    }
}

/*
-----------------------------------------------------------------------------
Comprehensive receipt function: 
Asks user for total line items received
Compares items against previous inventory
    If item matches, quantity and cost are updated
    Else new item, quatity, and cost are added 
-----------------------------------------------------------------------------
*/

void receiveGroceries(int& totalBins, Provision*& pantry)
{
    int receiptItems;
    std::cout << "Count line items on the receipt: ";
    std::cin >> receiptItems;
    std::cin.ignore();  // clear newline from buffer

    for (int i = 0; i < receiptItems; ++i)
    {
        std::string name;
        int receivedQty;
        double receivedCost;

        cout << "\nItem " << (i + 1) << " name: ";
        getline(std::cin, name);

        cout << "Total received: ";
        cin >> receivedQty;

        cout << "Cost: ";
        cin >> receivedCost;
        cin.ignore();  // clear buffer

        bool found = false;

        for (int j = 0; j < totalBins; ++j)
        {
            if (pantry[j].getName() == name)
            {
                pantry[j].setQuant((pantry[j].getQuant()) + receivedQty);
                if (receivedCost > 0)
                    pantry[j].setCost(receivedCost);

                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cout << "Adding new item to inventory: " << name << "\n";

            Provision* newPantry = new Provision[totalBins + 1];
            for (int j = 0; j < totalBins; ++j)
                newPantry[j] = pantry[j];

            newPantry[totalBins].setName(name);
            newPantry[totalBins].setQuant(receivedQty);
            newPantry[totalBins].setCost(receivedCost);

            delete[] pantry;
            pantry = newPantry;
            ++totalBins;
        }
    }

    // Write updated pantry to CSV
    std::ofstream outFile("pantry.csv");
    if (!outFile)
    {
        std::cerr << "Error opening inventory.csv for writing.\n";
        return;
    }

    for (int i = 0; i < totalBins; ++i)
    {
        outFile << pantry[i].getName() << "," << pantry[i].getQuant() << "," << pantry[i].getCost() << "\n";
    }

    outFile.close();
    cout << "\nInventory updated and saved to pantry.csv.\n";

}
