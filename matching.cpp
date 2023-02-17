/*
Peter Pham
17 Feb 2023
EECS 348 - Extra Credit Lab01
*/


#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

// file_IO, returns contents as a single 2D vector to be split later
vector<vector<int>> read_file(const char *file_name)
{
    vector<vector<int>> file_data;
    FILE *fp = fopen(file_name, "r+");
    if (fp == NULL)
    {
        printf("Incorrect/incompatible input file.");
    }

    for (int row = 0; row<10; row++)
    {
        vector<int> vector_row;
        for (int column=0; column<5; column++)
        {
            int preference_num;
            fscanf(fp, "%i\n", &preference_num);
            // counts preferences starting at 0 to make getting indexes much easier
            vector_row.push_back((preference_num-1));
        }
        file_data.push_back(vector_row);
    }
    return file_data;
}

// value_index required or else the function would count the input value itself and always return true
bool in_vector(const vector<int> v, int value, int value_index) // checks if more than one instance of [value] exists in the input vector
{
    bool is_in = false;
    for (int i = 0; i < v.size(); i++)
    {
        if (v[i] == value && i != value_index)
        {
            is_in = true;
        }
    }
    return is_in;
}

void debug_print_vector(const vector<int> v)
{
    printf("===DEBUG PRINT===\n");
    for (int i = 0; i < v.size(); i++)
    {
        printf("%i ", v[i]);
    }
    cout << "===========" << endl;
}

vector<int> get_index_of(const vector<int> v, int value) // iterates through the input vector and returns all indexes of the test value
{
    vector<int> indexes;
    for (int i = 0; i < v.size(); i++)
    {
        if (v[i] == value)
        {
            indexes.push_back(i);
        }
    }
    return indexes;
}

// alternative way for assigning departments by choosing first available option from the department prefs
vector<int> dept_selection(vector<int> dept_prefs, vector<int> department_choices, int dept_id) 
{
    for (int i = 0; i < dept_prefs.size(); i++)
    {
        if ((in_vector(department_choices, dept_prefs[i], -1) == 0) && (in_vector(department_choices, dept_prefs[i], -1) == 0))
        {
            department_choices[dept_id] = dept_prefs[i];
            return department_choices;
        }
    }
    return department_choices;
}

// returns an individual dept or programmer's perf in the form of a vector
vector<int> get_column_prefs(const vector<vector<int>> v, int column)
{
    vector<int> column_prefs;
    for (int row = 0; row < v.size(); row++)
    {
        column_prefs.push_back(v[row][column]);
    }
    return column_prefs;
}

// used to resolve duplicates in a department's preferences
vector<int> resolve_conflicts(vector<int> dupe_dept_choice_id, vector<vector<int>> dupe_prefs, vector<int> department_choices, int row, vector<vector<int>> all_programmer_prefs)
{
    int dupe_programmer_id = dupe_prefs[0][row];
    vector<int> dupe_programmer_prefs = get_column_prefs(all_programmer_prefs, dupe_programmer_id);
    // iterates through the programmer's preferences
    for (int pref_index = 0; pref_index < dupe_programmer_prefs.size(); pref_index++)
    {
        // iterates through the duplicate department ids
        for (int dept_id = 0; dept_id < dupe_dept_choice_id.size(); dept_id++)
        {
            // checks if a match exists. if not, continue to the next programmer preference until one is found
            if (dupe_programmer_prefs[pref_index] == dupe_dept_choice_id[dept_id])
            {
                // once found, make the assignment in the choices list (vector type)
                department_choices[dupe_dept_choice_id[dept_id]] = dupe_programmer_id;
                // iterate through the other departments and assign them the next available programmer based off their preferences.
                for (int tmp = 0; tmp < dupe_dept_choice_id.size(); tmp++)
                {
                    if (tmp != dept_id) // ensures the dept that was just assigned is ignored
                    {
                        department_choices = dept_selection(dupe_prefs[tmp], department_choices, dupe_dept_choice_id[tmp]);
                    }
                }
                return department_choices;
            }
        }
    }
    return department_choices;
}

void print_output(vector<int> department_choices)
{
    for (int i = 0; i < department_choices.size(); i++)
    {
        // +1 is added to all of the values as 1 is subtracted during the file_io for easier index reading/assigning
        printf("Department #%i will get Programmer #%i\n", i+1, (department_choices[i]+1));
    }
}

int main()
{
    // initializes vectors of vectors (2D Vectors)
    vector<vector<int>> input_data = read_file("input.txt");
    vector<vector<int>> all_department_prefs;
    vector<vector<int>> all_programmer_prefs;


    // splits the input data into the two preference groups
    for (int i = 0; i < 5; i++)
    {
        all_department_prefs.push_back(input_data[i]);
    }
    for (int i = 5; i < 10; i++)
    {
        all_programmer_prefs.push_back(input_data[i]);
    }

    // department choices are stored in a vector
    vector<int> department_choices = {-1, -1, -1, -1, -1};
    for (int i = 0; i < 5; i++)
    {
        vector<int> dept_row_prefs;
        for (int j = 0; j < 5; j++)
        {
            dept_row_prefs.push_back(all_department_prefs[i][j]);
        }

        // checks through the departments for ones without a tie and makes the assignment
        for (int dept = 0; dept < dept_row_prefs.size(); dept++)
        {
            if (department_choices[dept] == -1)
            {
                if ((in_vector(department_choices, dept_row_prefs[dept], dept) == 0) && (in_vector(dept_row_prefs, dept_row_prefs[dept], dept) == 0))
                {
                    department_choices[dept] = dept_row_prefs[dept];
                }
            }
        }

        // for ones with a tie, iterate through them
        for (int dept = 0; dept < dept_row_prefs.size(); dept++)
        {
            // ensures departments are not already assigned a programmer
            if (department_choices[dept] == -1)
            {
                // gets id (indexes) of the tied departments
                vector<int> dupe_dept_choice_id = get_index_of(dept_row_prefs, dept_row_prefs[dept]);

                // creates a 2D vector of the duplicate department's preferences
                vector<vector<int>> dupe_prefs;
                for (int i = 0; i < dupe_dept_choice_id.size(); i++)
                {
                    dupe_prefs.push_back(get_column_prefs(all_department_prefs, dupe_dept_choice_id[i]));
                }

                // gets the programmer's preferences
                vector<int> column_prefs = get_column_prefs(all_programmer_prefs, dept_row_prefs[dept]);


                for (int row = 0; row < dupe_prefs[i].size(); row ++)
                {
                    // ensures that the tied programmer is not already assigned to someone 
                    if (in_vector(department_choices, dupe_prefs[i][row], row) == 0)
                    {
                        department_choices = resolve_conflicts(dupe_dept_choice_id, dupe_prefs, department_choices, row, all_programmer_prefs);
                        break;
                    }
                }
            }
        }
    }
    print_output(department_choices);
    return 0;
}