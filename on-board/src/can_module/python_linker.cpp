#include<Python.h>
#include<stdlib.h>
#include<iostream>
#include<map>

int main()
{
    PyObject *module_name, *module, *dict, *python_class, *object;

    // Initializes the Python interpreter
    Py_Initialize();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");

    module_name = PyUnicode_FromString(
        "python_module");

    // Load the module object
    module = PyImport_Import(module_name);
    if (module == nullptr) {
        PyErr_Print();
        std::cerr << "Fails to import the module.\n";
        return 1;
    }
    Py_DECREF(module_name);

    // dict is a borrowed reference.
    dict = PyModule_GetDict(module);
    if (dict == nullptr) {
        PyErr_Print();
        std::cerr << "Fails to get the dictionary.\n";
        return 1;
    }
    Py_DECREF(module);

    // Builds the name of a callable class
    python_class = PyDict_GetItemString(dict, "CAN_Decoder");
    if (python_class == nullptr) {
        PyErr_Print();
        std::cerr << "Fails to get the Python class.\n";
        return 1;
    }
    Py_DECREF(dict);

    // Creates an instance of the class
    if (PyCallable_Check(python_class)) {
        PyObject *py_args = PyTuple_New(1);
		PyTuple_SetItem(py_args, 0, PyUnicode_FromString("./DFLOW.dbc"));
        object = PyObject_CallObject(python_class, py_args);
        Py_DECREF(python_class);
    } else {
        std::cout << "Cannot instantiate the Python class" << std::endl;
        Py_DECREF(python_class);
        return 1;
    }

    while(true)
    {
        std::map<std::string, std::string> my_map;

        PyObject *pDict = PyObject_CallMethod(object, "get_message","()"); 

        if(pDict != nullptr)
        { 
            PyObject *pKeys = PyDict_Keys(pDict);
            PyObject *pValues = PyDict_Values(pDict);

            for (Py_ssize_t i = 0; i < PyDict_Size(pDict); ++i) 
            {
                std::string key = std::string(PyUnicode_AsUTF8( PyList_GetItem(pKeys,   i) ));
                std::string value = std::string(PyUnicode_AsUTF8( PyList_GetItem(pValues,   i) ));
                my_map.insert( std::pair<std::string, std::string>(key,value));
            }

            if(my_map["MessageType"] == "IMUSensor")
                std::cout<<"LeanAngle "<< my_map["LeanAngle"]<<std::endl;
            if(my_map["MessageType"] == "EngineSensors")
                std::cout<<"WaterTemp "<< my_map["WaterTemperature"]<<std::endl;
        }
    }

    //std::getchar();
    Py_Finalize();

    return (0);
}