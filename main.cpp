/*
 ============================================================================
 Name        : main.cpp
 Author      : BISHOY KAMEL & MOHAMED ELANSRI & ENG JOE & SHAHD EHAB
 Date        : 3-12-2023
 Copyright   : Your copyright notice
 Description : Shop Management System Project Using C++
 ============================================================================
 */

/*******************************************************************************
*                                  Inclusions                                  *
*******************************************************************************/


#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <conio.h>
#include <thread>
#include <chrono>
#include <cctype>
#include <Windows.h>
#include "lib/json.hpp"
using namespace std;
using json = nlohmann::json;


/*******************************************************************************
*                              Global Variables	                               *
*******************************************************************************/

string prodDB="./db/products.json";
string vaultDB="./db/vault.txt";
string invoiceDB="./db/invoice.txt";


/*******************************************************************************
*                             Function declaration                             *
*******************************************************************************/

void clearScreenWithDelay(int delayMilliseconds);
void coutt(const string& text, int millisecondsPerChar);
bool isFloat(const string& s);


/*******************************************************************************
*                           Class declaration & definition                    *
*******************************************************************************/


/*******************************************************************************
*                    Product Class declaration & definition                    *
*******************************************************************************/


class Product {
public:
    Product() : id(""), name(""), buyingPrice(0.0), sellingPrice(0.0), quantity(0) {}
    Product(const string& id,const string& name, double buyingPrice, double sellingPrice, int quantity)
        : id(id), name(name), buyingPrice(buyingPrice), sellingPrice(sellingPrice), quantity(quantity) {}

    // Setters & Getters 
    string getId() const { return id; }
    string getName() const { return name; }
    double getBuyingPrice() const { return buyingPrice; }
    double getSellingPrice() const { return sellingPrice; }
    int getQuantity() const { return quantity; }

    void setName(const string& newName) { name = newName; }
    void setBuyingPrice(double newBuyingPrice) { buyingPrice = newBuyingPrice; }
    void setSellingPrice(double newSellingPrice) { sellingPrice = newSellingPrice; }
    void setQuantity(int newQuantity) { quantity = newQuantity; }
    void setID(const string& newID) { id = newID; }

    double calculateTotalCost(int quantity) const {
        return buyingPrice * quantity;
    }

    double calculateTotalSellingPrice(int quantity) const {
        return sellingPrice * quantity;
    }
    void addQuantity(int newQuantity) { quantity += newQuantity; }

private:
    string id;
    string name;
    double buyingPrice;
    double sellingPrice;
    int quantity;
};

/*******************************************************************************
*                    Invoice Class declaration & definition                    *
*******************************************************************************/

class Invoice {
    public:
    Invoice(const string& id, const string& type, const vector<pair<string, int>>& items)
        : id(id), type(type), items(items) {}

    string getId() const { return id; }
    string getType() const { return type; }
    const vector<pair<string, int>>& getItems() const { return items; }

    double calculateTotalCost(const vector<Product>& products) const {
        double totalCost = 0.0;
        for (const auto& item : items) {
            auto it = find_if(products.begin(), products.end(),
                             [productId = item.first](const Product& p) { return p.getId() == productId; });

            if (it != products.end()) {
                totalCost += it->calculateTotalCost(item.second);
            } else {
                throw runtime_error("Product with ID " + item.first + " not found.");
            }
        }
        return totalCost;
    }

    double calculateTotalSellingPrice(const vector<Product>& products) const {
        double totalSellingPrice = 0.0;
        for (const auto& item : items) {
            auto it = find_if(products.begin(), products.end(),
                             [productId = item.first](const Product& p) { return p.getId() == productId; });

            if (it != products.end()) {
                totalSellingPrice += it->calculateTotalSellingPrice(item.second);
            } else {
                throw runtime_error("Product with ID " + item.first + " not found.");
            }
        }
        return totalSellingPrice;
    }

private:
    string id;
    string type; // "sell" or "buy"
    vector<pair<string, int>> items; // Product ID and quantity pairs
};

/*******************************************************************************
*                    Vault Class declaration & definition                      *
*******************************************************************************/

class Vault {
    private:
        string file_path;
        double balance;

        double loadBalance() {
            ifstream file(file_path);
            if (file.is_open()) {
                file >> balance;
                file.close();
            } else {
                balance = 0.0;
            }
            return balance;
        }

        void saveBalance() {
            ofstream file(file_path);
            if (file.is_open()) {
                file << fixed << setprecision(2) << balance;
                file.close();
            }
        }

    public:
        Vault(const string& filePath = vaultDB) : file_path(filePath) {
            balance = loadBalance();
        }

    void showBalance() const {
            coutt("Current Balance: $",25);cout << fixed << setprecision(2) << balance << endl;
        }

    void addMoney() {
        string amount;
        do {
            coutt("Enter the amount to add: $",25) ;
            joe:
            cin >> amount;
            if(!isFloat(amount)){coutt("Enter the amount to add and it must be a number : $",25); goto joe;}
            if (stod(amount) <= 0) {
                coutt("Invalid amount. Please enter a positive value.",25); cout << endl;
            }

        } while (stod(amount)  <= 0);

        balance += stod(amount);
        saveBalance();
        coutt("Added $",25); cout << fixed << setprecision(2) << stod(amount); coutt(" to the vault.",25); cout << endl;
        showBalance();
         }

    void takeMoney() {
        string amount;
        do {

            coutt("Enter the amount to withdraw: $",25) ;
            joe:
            cin >> amount;
            if(!isFloat(amount)){coutt("Enter the amount to withdraw and it must be a number : $",25); goto joe;}
            if (stod(amount) <= 0 || stod(amount) > balance) {
                coutt("Invalid amount. Please enter a positive value not exceeding the available balance.",25); cout <<endl;
            }

        } while (stod(amount) <= 0 || stod(amount) > balance);

        balance -= stod(amount);
        saveBalance();
        coutt("Withdrew $",25); cout << fixed << setprecision(2) << stod(amount); coutt(" from the vault.",25);cout << endl;
        showBalance();
    }

    void addMoneyfromSell(double amount) {
            balance += amount;
            saveBalance();
    }

    void takeMoneyfrombuy(double amount) {
        balance -= amount;
        saveBalance();
    }

};

/*******************************************************************************
*                 StorageSystem Class declaration & definition                 *
*******************************************************************************/

class StorageSystem {
public:
    void saveProductsToFile(const string& filename) const {
        json jsonProducts;

        // Read existing JSON data from the file, if it exists
        ifstream inputFile(filename);
        if (inputFile.is_open()) {
            inputFile >> jsonProducts;
            inputFile.close();
        }

        // Append new products to the existing JSON data
        for (const auto& product : products) {
            json jsonProduct;
            jsonProduct["id"] = product.getId();
            jsonProduct["name"] = product.getName();
            jsonProduct["buyingPrice"] = product.getBuyingPrice();
            jsonProduct["sellingPrice"] = product.getSellingPrice();
            jsonProduct["quantity"] = product.getQuantity();
            jsonProducts.push_back(jsonProduct);
        }

        // Write the updated JSON data to the file
        ofstream outputFile(filename, ios::trunc); // Open in truncation mode to clear existing content
        if (!outputFile.is_open()) {
            throw runtime_error("Error opening file: " + filename);
        }

        outputFile << setw(4) << jsonProducts << endl;
    }
    json initializeFromJsonFile(const string& jsonFileName) {
        json storageData;
        ifstream storageFile(jsonFileName);
        if (storageFile.is_open()) {
            storageFile >> storageData;
            storageFile.close();

            products.clear(); // Clear existing products

            for (const auto& productData : storageData) {
                addProduct(Product(
                    productData["id"],
                    productData["name"],
                    productData["buyingPrice"],
                    productData["sellingPrice"],
                    productData["quantity"]
                ));
            }
            return storageData;
        } else {
            throw runtime_error("Error opening file: " + jsonFileName);
        }
    }

    void appendProductToFile(const string& filename, const Product& newProduct) {
    ifstream inFile(filename);
    json existingJson;

    // Check if the file is empty or doesn't exist
    if (inFile.is_open()) {
        inFile >> existingJson;
        inFile.close();
    }

    json newProductJson;
    newProductJson["id"] = newProduct.getId();
    newProductJson["name"] = newProduct.getName();
    newProductJson["buyingPrice"] = newProduct.getBuyingPrice();
    newProductJson["sellingPrice"] = newProduct.getSellingPrice();
    newProductJson["quantity"] = newProduct.getQuantity();

    existingJson.push_back(newProductJson);

    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw runtime_error("Error opening file: " + filename);
    }

    outFile << setw(4) << existingJson << endl;
    }
    // pushes the product into the vector
    void addProduct(const Product& product) {
        products.push_back(product);
    }

    void removeProductById(const string& productId) {
        auto it = findProductById(productId);
        if (it != products.end()) {
            products.erase(it);
        } else {
            throw runtime_error("Product with ID " + productId + " not found.");
        }
    }
     void editProduct(const string& productId, const int& attributeName, const string& attributeValue) {
        auto it = findProductById(productId);
        if (it != products.end()) {
            if (attributeName == 1) {
                it->setName(attributeValue);
            } else if (attributeName == 3) {
                it->setBuyingPrice(stod(attributeValue));
            } else if (attributeName == 2) {
                it->setSellingPrice(stod(attributeValue));
            } else if (attributeName == 4) {
                it->setQuantity(stoi(attributeValue));
            } else {
                throw runtime_error("Invalid attribute name: " + attributeName);
            }
        } else {
            throw runtime_error("Product with ID " + productId + " not found.");
        }
    }

    Product getProductById(const string& productId) const {
        auto it = findProductById(productId);
        if (it != products.end()) {
            return *it;
        } else {
            throw runtime_error("Product with ID " + productId + " not found.");
        }
    }

    Product getProductByName(const string& productName) const {
        auto it = findProductByName(productName);
        if (it != products.end()) {
            return *it;
        } else {
            throw runtime_error("Product with name " + productName + " not found.");
        }
    }

    void printAllProducts() const {
        for (const auto& product : products) {
             cout << "ID: " << product.getId() << ", Name: " << product.getName()
                      << ", Buying Price: $" << fixed << setprecision(2) << product.getBuyingPrice()
                      << ", Selling Price: $" << fixed << setprecision(2) << product.getSellingPrice()
                      << ", Quantity: " << product.getQuantity() << "\n";
        }
    }

    // Helper function to find a product by ID
    vector<Product>::iterator findProductById(const string& productId) {
        return find_if(products.begin(), products.end(),
                            [productId](const Product& p) { return p.getId() == productId; });
    }

    // Helper function to find a product by ID (const version)
    vector<Product>::const_iterator findProductById(const string& productId) const {
        return find_if(products.begin(), products.end(),
                            [productId](const Product& p) { return p.getId() == productId; });
    }

    // Helper function to find a product by name
    vector<Product>::iterator findProductByName(const string& productName) {
        return find_if(products.begin(), products.end(),
                            [productName](const Product& p) { return p.getName() == productName; });
    }

    // Helper function to find a product by name (const version)
    vector<Product>::const_iterator findProductByName(const string& productName) const {
        return find_if(products.begin(), products.end(),
                            [productName](const Product& p) { return p.getName() == productName; });
    }
    void addquantity(const string& productId,int quantity) {
        auto it = findProductById(productId);
        if (it != products.end()) {
                it->addQuantity(quantity);
            } else {
            throw runtime_error("Product with ID " + productId + " not found.");
        }
    }
    bool isIDUsed(const string& filePath, const string& id) {
    ifstream file(filePath);
    if (file.is_open()) {
        json products;
        file >> products;

        for (const auto& product : products) {
            if (product["id"] == id) {
                return true; // ID is already used
            }
        }
    }
    return false; // ID is not used
    }
    vector<Product> getProducts(){
        return products;
    }

private:
    vector<Product> products;
};

/*******************************************************************************
*                 SalesSystem Class declaration & definition                   *
*******************************************************************************/

class SalesSystem {
    public:
    SalesSystem(StorageSystem& storage, Vault& vault)
        : storage(storage), vault(vault) {
    }
    void sellProduct(const string& productId, int quantity) {
        auto it = storage.findProductById(productId);
        if (it != storage.getProducts().end()) {

            int newQuantity=it->getQuantity() -quantity;

            string id =it->getId();

            storage.editProduct(id,4,to_string(newQuantity));

            ofstream clearFile(prodDB, ios::trunc);

            json jsonArray = json::array();
            ofstream outputFile(prodDB);
            outputFile << setw(4) << jsonArray << endl;
            outputFile.close();

            storage.saveProductsToFile(prodDB);
                vector<pair<string, int>> soldItems = {{productId, quantity}};
                double totalSellingPrice = it->calculateTotalSellingPrice(quantity);
                vault.addMoneyfromSell(totalSellingPrice);
            }else {
            throw runtime_error("Product with ID " + productId + " not found.");
        }
    }

    void buyProduct(const string& productId, int quantity) {
        auto it = storage.findProductById(productId);
        if (it != storage.getProducts().end()) {
            it->setQuantity(it->getQuantity() + quantity);
            vector<pair<string, int>> boughtItems = {{productId, quantity}};
            double totalCost = it->calculateTotalCost(quantity);
            vault.takeMoneyfrombuy(totalCost);
        } else {
            throw runtime_error("Product with ID " + productId + " not found.");
        }
    }
    const vector<Invoice>& getSellingInvoices() const {
        return sellingInvoices;
    }
    const vector<Invoice>& getbuyingInvoices() const {
        return buyingInvoices;
    }
    private:
    StorageSystem& storage;
    Vault& vault;
    vector<Invoice> sellingInvoices;
    vector<Invoice> buyingInvoices;

};

/*******************************************************************************
*                             Function declaration                             *
*******************************************************************************/

void saveInvoiceToJsonFile(const SalesSystem& sa, const std::string& filename);

int login();

bool isInteger(const string& s);

int printcashermenu();

void Createsalesinvoice(StorageSystem s, Vault v, SalesSystem sa) ;

int printaccountermenu();

void accounter();

void addProducts(); 

void displayProducts();

void editProduct();

void addStorage();

void updateJsonFile(const string& filename, const json& updates);

void storages();

void accounting();

int printcashermenu();

void casher();

void welcome_screen();

void playClickSound();

void stopCustomSound(int m);

void printToFile(const std::string& filename, const std::string& content);

void playcashSound();

/*******************************************************************************
*                             Main Function definition                         *
*******************************************************************************/
StorageSystem storage;
Vault mainvault(vaultDB);
SalesSystem sale(storage,mainvault);

int main() {
    system("color 0A");
    storage.initializeFromJsonFile(prodDB);
    welcome_screen();
    int User = login();
    clearScreenWithDelay(0);
    switch (User) {
    case 1:
       coutt("casher is open\n",20);
       clearScreenWithDelay(2000);
       casher();
        break;
    case 2:
        coutt("accounter is open\n",20);
        clearScreenWithDelay(2000);
        accounter();
        break;
    default:
        coutt("                            good bye (press a key to the program )",20);
        getchar();
    }
    return 0;
}

/*******************************************************************************
*                      Function declaration & definition                       *
*******************************************************************************/

int printcashermenu() {
       int c;
    do {
        coutt("[1]    Create a sales invoice\n",30);
        coutt("[2]    exit \n",30);
        coutt("Enter an option: ",30) ;
        cin >> c;
    } while (c < 0 || c > 2);
     return c;
}


void casher() {
    int c;
        c = printcashermenu();
        clearScreenWithDelay(0);
        switch (c) {
            case 1:
                Createsalesinvoice(storage,mainvault,sale);
                clearScreenWithDelay(500);
                casher();
                break;
            default:
            clearScreenWithDelay(0);
                coutt("                                            good bye (press a key to the program )",20); ;
                    getch();
        }
}

void Createsalesinvoice(StorageSystem s, Vault v, SalesSystem sa) {
    char choice;
    do {
    clearScreenWithDelay(200);
        try {
            // Get multiple products from the user in each session
            vector<pair<string, int>> productsToSell;
            
            while (true) {
                // Get product ID from the user
                coutt("Enter product ID (press 'd' to finish session): ",25);
                string productId;
                id:
                cin >> productId;
                // Check if the user wants to finish the session
                if (productId == "d" || productId == "D") {
                    break;
                }
                if(!storage.isIDUsed(prodDB,productId)) {coutt("ID is not used please enter anouther ID (press 'd' to finish session): ",25); goto id; }
                else{
                // Get quantity from the user
                stringerr:
                coutt("Enter quantity: ",25);
                    string quantity;
                    cin >> quantity;
                    if(!isInteger(quantity)){
                            coutt("Quantity must be a number\n",25); goto stringerr;
                    }
                    else{
                        // Add the product and quantity to the list
                    productsToSell.push_back({productId, stoi(quantity)});
                    } 
                }
            }

            // Print the details of each product and calculate total invoice price
            double totalInvoicePrice = 0.0;
            coutt("\nInvoice Details:\n",25);
            coutt("Product Name\t\t  Selling Price\t\tQuantity\tTotal Price\n",25);
            for (const auto& product : productsToSell) {
                const Product& soldProduct = s.getProductById(product.first);
                double totalProductPrice = soldProduct.calculateTotalSellingPrice(product.second);
                totalInvoicePrice += totalProductPrice;
            // Print product details
            cout << setw(30) << left << soldProduct.getName()
             << setw(21) << fixed << setprecision(2) << soldProduct.getSellingPrice()
             << setw(17) << product.second
             << setw(14) << totalProductPrice << endl;

            }

            // Print total invoice price
            coutt("Total Invoice Price: $",25);cout <<totalInvoicePrice << endl;

            // Sell the products
            for (const auto& product : productsToSell) {
                sa.sellProduct(product.first, product.second);
            }

            // Save changes to files after each successful sale
            v.addMoneyfromSell(totalInvoicePrice);

            // Call printToFile to append the content to the file
            printToFile(invoiceDB, "Invoice Details:\n");
            printToFile(invoiceDB, "Product Name   Selling Price          Quantity          Total Price\n");

            // Loop through the products and print each product's details to the file
            for (const auto& product : productsToSell) {
                    const Product& soldProduct = s.getProductById(product.first);
                    double totalProductPrice = soldProduct.calculateTotalSellingPrice(product.second);
                    string productDetails = soldProduct.getName() + "\t\t" + to_string(soldProduct.getSellingPrice()) + "\t\t" +
                                                to_string(product.second) + "\t\t" + to_string(totalProductPrice);
                    printToFile(invoiceDB, productDetails);
                }

            // Print the total invoice price to the file
                printToFile(invoiceDB, "Total Invoice Price: $" + to_string(totalInvoicePrice));


        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }

        // Clear the input buffer
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Ask if the user wants to continue selling products
        cout<<"Do you want to create another invoice? (y/n): ";
        playcashSound();
        cin >> choice;
    
    } while (tolower(choice) == 'y');
    clearScreenWithDelay(0);
}

int printaccountermenu() {
       int c;
    do {
        coutt("[1]    storage\n",25) ;
        coutt("[2]    accounting\n",25) ;
        coutt("[3]    exit \n",25) ;
        coutt("Enter an option: ",25) ;
        cin >> c;
    } while (c < 0 || c > 3);
     return c;
}

void storages() {
    int c=0;
    do {
        coutt("[1]    add product\n",25) ;
        coutt("[2]    display product\n",25) ;
        coutt("[3]    edit product\n",25) ;
        coutt("[4]    add storge\n",25) ;
        coutt("[5]    exit\n",25) ;
        coutt("Enter an option: ",25) ;
        cin >> c;
        clearScreenWithDelay(0);
        switch (c) {
            case 1:
                addProducts();
                storages();
                break;
            case 2:
                displayProducts();
                storages();
                break;
            case 3:
                editProduct();
                storages();
                break;
            case 4:
                addStorage();
                storages();
                break;
            case 5: break;
            default:
                coutt("Invalid option. Please try again.\n",25) ;
        }
    } while (c < 0 || c > 5);

}

Product inputProd() {
    Product s1;
    string tx;
    //get product data
    coutt("Enter Product Data : \n",25) ;
    //get product id
    coutt("Product ID: ",25) ;
    id:
    cin >> tx;
    if(storage.isIDUsed(prodDB,tx)) {coutt("ID is already used please anouther ID: ",25); goto id; }
    else{
    s1.setID(tx);
    cout << endl;
    }
    //get product name
    coutt("Product Name: ",25);
    // Clear the input buffer
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin,tx);
    s1.setName(tx);
    cout << endl;


    coutt("Product Sell Price: ",25) ;
    SellingPrice:
    cin >> tx;
    if(!isFloat(tx)) {coutt("you must enter a number : ",25); goto SellingPrice; }
    else{
        s1.setSellingPrice(stof(tx));
        cout << endl;
    }

    coutt("Product Buy Price: ",25) ;
    Buy:
    cin >> tx;
    if(!isFloat(tx)) {coutt("you must enter a number : ",25); goto Buy; }
    else{
        s1.setBuyingPrice(stof(tx));
        cout << endl;
    }

    coutt("Product Quantity: ",25) ;
    Quantity:
    cin >> tx;
    if(!isInteger(tx)) {coutt("you must enter a number",25); goto Quantity; }
    else{
        s1.setQuantity(stoi(tx));
        cout << endl;
    }
    return s1;
}

void accounter() {
    int c;
        c = printaccountermenu();
        clearScreenWithDelay(0);
        switch (c) {
            case 1:
                storages();
                accounter();
                break;
            case 2:
                accounting();
                accounter();
                break;
            default:
                coutt("                            good bye (press a key to the program )",20);
                    getch();
        }
}

void addProducts() {
    char choice;
    do {
        clearScreenWithDelay(0);
        Product obj = inputProd();
        storage.addProduct(obj);
        storage.appendProductToFile(prodDB, obj);
        coutt("Do you want to add another Product? (y/n): ",25);
        cin >> choice;
    } while (tolower(choice) == 'y');
    clearScreenWithDelay(0);
}

void displayProducts() {
    clearScreenWithDelay(0);
    coutt("products is : \n",25);
    storage.printAllProducts();
}

void editProduct() {
    coutt( "Enter the product ID :",25);
    string iid;
    isIDUsed:
    cin >> iid;
    if(!storage.isIDUsed(prodDB,iid)) {coutt("ID is not found please enter anouther ID: ",25); goto isIDUsed; }
    int attrName;
    do {
        clearScreenWithDelay(0);
        coutt( "Choose the name of the attribute you want to change: \n",25);
        coutt( "[1] Product Name.\n",25);
        coutt( "[2] Product Sell Price\n",25);
        coutt( "[3] Product Buy Price.\n",25);
        coutt( "[4] Product Quantity.\n",25);
        cin >> attrName;
    } while (attrName < 1 || attrName > 4);

    coutt("Enter the new value of the attribute to change: ",25) ;
    string attrValue;
    cin >> attrValue;
    clearScreenWithDelay(0);
    // Update the product in memory
    storage.editProduct(iid, attrName, attrValue);
    ofstream clearFile(prodDB, ios::trunc);
    json jsonArray = json::array();
    ofstream outputFile(prodDB);
    outputFile << setw(4) << jsonArray << endl;
    outputFile.close();
    storage.saveProductsToFile(prodDB);
}

void addStorage() {
    string id,q;
    coutt("Enter the product ID :",25);
    isIDUsed:
    cin >> id;
    if(!storage.isIDUsed(prodDB,id)) {coutt("ID is not found please enter anouther ID: ",25); goto isIDUsed; }
    coutt("Enter the Quantity :",25);
    Quantity:
    cin >> q;
    if(!isInteger(q)) {coutt("you must enter a number",25); goto Quantity; }
    storage.addquantity(id,stoi(q));
    ofstream clearFile(prodDB, ios::trunc);
    json jsonArray = json::array();
    ofstream outputFile(prodDB);
    outputFile << setw(4) << jsonArray << endl;
    outputFile.close();
    storage.saveProductsToFile(prodDB);
    clearScreenWithDelay(0);
}

int login() {
    string Username;
    int pass;
    Wrong:
    coutt("\t\t\t\t\t\t*******************************************\n",30); stopCustomSound(0);
    coutt("\t\t\t\t\t\tUsername : ",30); stopCustomSound(0);
    cin >> Username;
    coutt("\t\t\t\t\t\tPassword : ",30); stopCustomSound(0);
    cin >> pass;
    coutt("\t\t\t\t\t\t*******************************************\n'",30); stopCustomSound(0);
    if (pass != 1234 || (Username != "accounter" && Username != "casher")) {
        coutt( "Wrong password or Username \n",30);
        goto Wrong;
    }
    if (Username == "casher")
        return 1;
    else
        return 2;
}

void accounting() {
    int c=0;
    do {
        coutt("[1]    show balance\n",25) ;
        coutt("[2]    add money\n",25) ;
        coutt("[3]    take money\n",25) ;
        coutt("[4]    exit\n",25) ;
        coutt("Enter an option: ",25) ;
        cin >> c;
        switch (c) {
            case 1:
                clearScreenWithDelay(0);
                mainvault.showBalance();
                accounting();
                break;
            case 2:
                clearScreenWithDelay(0);
                mainvault.addMoney();
                accounting();
                break;
            case 3:
                clearScreenWithDelay(0);
                mainvault.takeMoney();
                accounting();
                break;
            case 4: clearScreenWithDelay(0);break;
            default:
                coutt("Invalid option. Please try again.\n",25);
        }
    } while (c < 0 || c > 5);

}

void playClickSound() {
    // Adjust the sound file path based on your system
    PlaySound(TEXT("./lib/type.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void playcashSound() {
    // Adjust the sound file path based on your system
    PlaySound(TEXT("./lib/cash.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void stopCustomSound(int m) {
    // Introduce a delay (adjust as needed)
    Sleep(m);  // 1000 milliseconds (1 second) delay
}

void stopCustomSound() {
    // Stop the sound immediately
    PlaySound(NULL, 0, SND_PURGE);
}

void coutt(const string& text, int millisecondsPerChar) {
        playClickSound();  // Play custom sound after each character
    for (char c : text) {
        
        cout << c << flush;
        auto start = chrono::steady_clock::now();
        auto end = start + chrono::milliseconds(millisecondsPerChar);

        while (chrono::steady_clock::now() < end) {
            // Wait for the desired time
        }
        
    }
}

bool isInteger(const string& s) {
    // Check if each character in the string is a digit
    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool isFloat(const string& s) {
    // Check if each character in the string is a digit
    for (char c : s) {
        if (!isdigit(c)&&c!='.') {
            return false;
        }
    }
    return true;
}

void welcome_screen()
{
	coutt("\t\t\t\t\t\t###########################################\n",10);
	coutt("\t\t\t\t\t\t###########################################\n",10);
	coutt("\t\t\t\t\t\t## Welcome to our shop management system ##\n",20);
	coutt("\t\t\t\t\t\t###########################################\n",10);
	coutt("\t\t\t\t\t\t###########################################\n",10);
	stopCustomSound();cout <<"\n";

}

void printToFile(const std::string& filename, const std::string& content) {
    // Open the file in append mode
    ofstream file(filename, ios::app);

    // Check if the file is open
    if (file.is_open()) {
        // Write the content to the file
        file << content << endl;

        // Close the file
        file.close();
    } else {
        // Display an error message if the file cannot be opened
        cerr << "Error opening file: " << filename << std::endl;
    }
}

void clearScreenWithDelay(int milliseconds) {
    // Introduce a delay
    clock_t start_time = clock();
    while (((clock() - start_time) * 1000 / CLOCKS_PER_SEC) < milliseconds) {
        // Busy-wait until the required time has passed
    }
    // Clear the screen
    system("cls");
}