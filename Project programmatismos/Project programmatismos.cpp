#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <filesystem> // Required for directory operations

using namespace std;
namespace fs = std::filesystem; // Alias for std::filesystem

// Δομή για την αποθήκευση μιας συνταγής
struct Recipe {
    string name;
    string category;
    vector<string> ingredients;
    string instructions;
    int preparationTime;
};

// Function για την ανάγνωση μιας συνταγής από ένα αρχείο .txt
Recipe readRecipeFromFile(const string& filename) {
    Recipe recipe;
    ifstream inputFile(filename);
    if (inputFile.is_open()) {
        string line;
        string currentSection;

        while (getline(inputFile, line)) {
            if (line.find("Name:") == 0) {
                recipe.name = line.substr(string("Name:").length() + 1);
            }
            else if (line.find("Category:") == 0) {
                recipe.category = line.substr(string("Category:").length() + 1);
            }
            else if (line.find("Preparation Time:") == 0) {
                recipe.preparationTime = stoi(line.substr(string("Preparation Time:").length() + 1));
            }
            else if (line == "Ingredients:") {
                currentSection = "Ingredients";
            }
            else if (line == "Instructions:") {
                currentSection = "Instructions";
            }
            else if (currentSection == "Ingredients") {
                recipe.ingredients.push_back(line);
            }
            else if (currentSection == "Instructions") {
                recipe.instructions += line + "\n";
            }
        }
        inputFile.close();
    }
    else {
        cerr << "I couldn't open the file: " << filename << endl;
    }
    return recipe;
}

// Function για την προβολή μιας συγκεκριμένης συνταγής
void viewRecipe(const string& filename) {
    Recipe recipe = readRecipeFromFile(filename);
    cout << "\n--- " << recipe.name << " ---" << endl;
    cout << "Category: " << recipe.category << endl;
    cout << "Preperation Time: " << recipe.preparationTime << " minutes" << endl;
    cout << "Ingredients:" << endl;
    for (const string& ingredient : recipe.ingredients) {
        cout << "- " << ingredient << endl;
    }
    cout << "\nInstruction:" << endl;
    cout << recipe.instructions << endl;
    cout << "----------------------" << endl;
}

// Function για την εμφάνιση των συνταγών ανά κατηγορία και την επιλογή συνταγής για προβολή
void viewRecipesByCategory(const vector<string>& recipeFiles) {
    if (recipeFiles.empty()) {
        cout << "There are no recipe files." << endl;
        return;
    }

    vector<Recipe> breakfastRecipes;
    vector<Recipe> mealRecipes;
    vector<Recipe> dessertRecipes;
    vector<Recipe> otherRecipes;

    for (const string& filename : recipeFiles) {
        Recipe recipe = readRecipeFromFile(filename);
        if (recipe.category == "Breakfast") {
            breakfastRecipes.push_back(recipe);
        }
        else if (recipe.category == "Meals") {
            mealRecipes.push_back(recipe);
        }
        else if (recipe.category == "Desserts") {
            dessertRecipes.push_back(recipe);
        }
        else {
            otherRecipes.push_back(recipe);
        }
    }

    int categoryChoice;
    cout << "\n--- View Recipes ---" << endl;
    cout << "Choose Category:" << endl;
    cout << "1. Breakfast" << endl;
    cout << "2. Meals" << endl;
    cout << "3. Desserts" << endl;
    cout << "0. Return to Main Menu" << endl;
    cout << "Choice: ";
    cin >> categoryChoice;

    vector<Recipe>* selectedCategory = nullptr;
    string categoryName;

    switch (categoryChoice) {
    case 1:
        system("cls"); // Clear screen after choosing Breakfast
        selectedCategory = &breakfastRecipes;
        categoryName = "Breakfast";
        break;
    case 2:
        system("cls"); // Clear screen after choosing Meals
        selectedCategory = &mealRecipes;
        categoryName = "Meals";
        break;
    case 3:
        system("cls"); // Clear screen after choosing Desserts
        selectedCategory = &dessertRecipes;
        categoryName = "Desserts";
        break;
    case 0:
        return;
    default:
        cout << "Wrong category choice." << endl;
        return;
    }

    if (selectedCategory != nullptr) {
        if (selectedCategory->empty()) {
            cout << "There are no recipes in this category '" << categoryName << "'." << endl;
        }
        else {
            cout << "\n--- Recipes " << categoryName << " ---" << endl;
            for (int i = 0; i < selectedCategory->size(); ++i) {
                cout << i + 1 << ". " << (*selectedCategory)[i].name << endl;
            }

            int recipeChoice;
            cout << "Choose the number of the recipe you want to view (0 to return to category menu): ";
            cin >> recipeChoice;

            if (recipeChoice > 0 && recipeChoice <= selectedCategory->size()) {
                string filenameToView = "";
                for (const string& file : recipeFiles) {
                    if (readRecipeFromFile(file).name == (*selectedCategory)[recipeChoice - 1].name) {
                        filenameToView = file;
                        break;
                    }
                }
                if (!filenameToView.empty()) {
                    system("cls"); // Clear screen before viewing the recipe
                    viewRecipe(filenameToView);
                    int backToCategory;
                    cout << "\nType 0 to go back to the category menu: ";
                    cin >> backToCategory;
                    if (backToCategory == 0) {
                        system("cls"); // Clear screen before showing the category menu again
                        viewRecipesByCategory(recipeFiles); // Re-display the category menu
                    }
                }
                else {
                    cout << "There are no files for this recipe." << endl;
                }
            }
            else if (recipeChoice != 0) {
                cout << "Wrong Choice" << endl;
            }
        }
    }
}

// Function για την αναζήτηση συνταγών με βάση τον μέγιστο χρόνο παρασκευής
void findRecipesByPreparationTime(const vector<string>& recipeFiles, int maxTime) {
    vector<Recipe> matchingRecipes;
    for (const string& filename : recipeFiles) {
        Recipe recipe = readRecipeFromFile(filename);
        if (recipe.preparationTime <= maxTime) {
            matchingRecipes.push_back(recipe);
        }
    }

    if (matchingRecipes.empty()) {
        cout << "There are no recipes with preparation time of " << maxTime << " minutes." << endl;
        int backToMain;
        cout << "\nType 0 to go back to the main menu: ";
        cin >> backToMain;
        if (backToMain == 0) {
            system("cls"); // Clear screen before returning to the main menu (loop will handle re-display)
        }
    }
    else {
        cout << "\n--- Recipes with maximum preparation time of " << maxTime << " minutes ---" << endl;
        for (const auto& recipe : matchingRecipes) {
            // Corrected line: Access recipe data directly
            cout << "- " << recipe.name << " (Category: " << recipe.category << ", Preparation Time: " << recipe.preparationTime << " minutes)" << endl;
        }
        cout << "---------------------------------------------------------" << endl;
        int backToMain;
        cout << "\nType 0 to go back to the main menu: ";
        cin >> backToMain;
        if (backToMain == 0) {
            system("cls"); // Clear screen before returning to the main menu (loop will handle re-display)
        }
    }
}

// Function για την προσθήκη μιας νέας συνταγής από τον χρήστη ΚΑΙ την εγγραφή της στο αρχείο
void addNewRecipe(vector<string>& recipeFiles) {
    Recipe newRecipe;
    string ingredient;

    cout << "\n--- Input New Recipe ---" << endl;
    cout << "Input the name of the recipe: ";
    cin.ignore();
    getline(cin, newRecipe.name);

    cout << "Input the category (Breakfast, Meals, Desserts): ";
    getline(cin, newRecipe.category);

    cout << "Input the preparation time in minutes: ";
    cin >> newRecipe.preparationTime;
    cin.ignore();

    cout << "Input the ingredients (one per line, type 'end' to finish):" << endl;
    while (getline(cin, ingredient) && ingredient != "end") {
        newRecipe.ingredients.push_back(ingredient);
    }

    cout << "Input the instructions (complete with an empty line):" << endl;
    string line;
    while (getline(cin, line) && !line.empty()) {
        newRecipe.instructions += line + "\n";
    }

    string filename = newRecipe.name;
    replace(filename.begin(), filename.end(), ' ', '_');
    filename += ".txt";

    ofstream outputFile(filename);
    if (outputFile.is_open()) {
        outputFile << "Name: " << newRecipe.name << endl;
        outputFile << "Category: " << newRecipe.category << endl;
        outputFile << "Preparation Time: " << newRecipe.preparationTime << endl;
        outputFile << "Ingredients:" << endl;
        for (const string& ing : newRecipe.ingredients) {
            outputFile << ing << endl;
        }
        outputFile << "Instructions:" << endl;
        outputFile << newRecipe.instructions;
        outputFile.close();
        cout << "The recipe '" << newRecipe.name << "' was saved in file'" << filename << "'." << endl;
        // Add the new filename to the vector of recipe files
        recipeFiles.push_back(filename);
    }
    else {
        cerr << "I couldn't open/create the file: " << filename << endl;
    }
    int backToMain;
    cout << "\nType 0 to go back to the main menu: ";
    cin >> backToMain;
    if (backToMain == 0) {
        system("cls"); // Clear screen before returning to the main menu (loop will handle re-display)
    }
}

int main() {
    // List to store the names of recipe files
    vector<string> recipeFiles;

    // Dynamically populate recipeFiles by scanning the current directory
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            // Read the recipe name from the file to check if it's a valid recipe file
            Recipe tempRecipe = readRecipeFromFile(entry.path().string());
            if (!tempRecipe.name.empty()) { // If the recipe name is not empty, it's likely a valid recipe file
                recipeFiles.push_back(entry.path().filename().string());
            }
        }
    }

    int choice;

    do {
        system("cls"); // Clear screen at the beginning of the loop
        cout << "\n---Welcome to Dalia's Recipe Book ---" << endl;
        cout << "1. View Recipe" << endl;
        cout << "2. Search For Recipes Based on Preparation Time" << endl;
        cout << "3. Input a New Recipe" << endl;
        cout << "0. Exit" << endl;
        cout << "Select an Action: ";
        cin >> choice;

        switch (choice) {
        case 1:
            system("cls"); // Clear screen after choosing "View Recipe"
            viewRecipesByCategory(recipeFiles);
            break;
        case 2: {
            system("cls"); // Clear screen after choosing "Search by Time"
            int maxTime;
            cout << "Input the Maximum Preparation Time: ";
            cin >> maxTime;
            if (cin.fail() || maxTime <= 0) {
                cout << "Wrong Input." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            else {
                system("cls"); // Clear screen before showing search results
                findRecipesByPreparationTime(recipeFiles, maxTime);
            }
            break;
        }
        case 3:
            system("cls"); // Clear screen after choosing "Input New Recipe"
            addNewRecipe(recipeFiles); // Pass recipeFiles by reference so it can be updated
            break;
        case 0:
            system("cls"); // Clear screen before exiting
            cout << "Thank You For Using Dalia's Recipe Book!" << endl;
            break;
        default:
            cout << "Wrong Input. Please Try Again" << endl;
        }
    } while (choice != 0);

    return 0;
}