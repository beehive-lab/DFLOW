#include "pycan_interface.hpp"


void CAN_Python_Interface::initializeInterface(std::string dbc_file_path, std::string python_file_path)
{
    // Initializes the Python interpreter
    Py_Initialize();
    PyObject *module_name, *module, *dict, *python_class, *object;

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");

    module_name = PyUnicode_FromString(
        "python_module");

    // Load the module object
    module = PyImport_Import(module_name);
    if (module == nullptr) {
        PyErr_Print();
        std::cout << "Fails to import the module.\n"<<std::endl;
    }
    Py_DECREF(module_name);

    // dict is a borrowed reference.
    dict = PyModule_GetDict(module);
    if (dict == nullptr) {
        PyErr_Print();
        std::cout << "Fails to get the dictionary.\n"<<std::endl;
    }
    Py_DECREF(module);

    // Builds the name of a callable class
    python_class = PyDict_GetItemString(dict, "CAN_Decoder");
    if (python_class == nullptr) {
        PyErr_Print();
        std::cout << "Fails to get the Python class.\n"<<std::endl;
    }
    Py_DECREF(dict);

    // Creates an instance of the class
    if (PyCallable_Check(python_class)) {
        PyObject *py_args = PyTuple_New(1);
		PyTuple_SetItem(py_args, 0, PyUnicode_FromString(dbc_file_path.c_str()));
        object = PyObject_CallObject(python_class, py_args);
        Py_DECREF(python_class);
    } else {
        std::cout << "Cannot instantiate the Python class" << std::endl;
        Py_DECREF(python_class);
    }

    if (object != nullptr)
        CAN_Python_Interface::python_can_class = object;
    else
    {
        std::cout << "Error instantiating class" << std::endl;
        throw 5;
    }

} 

std::map<std::string, std::string> CAN_Python_Interface::getMessageMap()
{
    //check there is a class instantiated
    if(CAN_Python_Interface::python_can_class == nullptr)
    {
        std::cerr<<"Error, python module not initialized"<<std::endl;
        throw 5;
    }
    else
    {
        std::map<std::string, std::string> message_map;

        //call python method from the python class-returns a dictionary
        PyObject *pDict = PyObject_CallMethod(CAN_Python_Interface::python_can_class, "get_message","()"); 

        if(pDict != nullptr)
        { 
            //convert dictionary to C++ map
            PyObject *pKeys = PyDict_Keys(pDict);
            PyObject *pValues = PyDict_Values(pDict);

            for (Py_ssize_t i = 0; i < PyDict_Size(pDict); ++i) 
            {
                std::string key = std::string(PyUnicode_AsUTF8( PyList_GetItem(pKeys,   i) ));
                std::string value = std::string(PyUnicode_AsUTF8( PyList_GetItem(pValues,   i) ));
                message_map.insert( std::pair<std::string, std::string>(key,value));
            }
        }
        else
        {
            std::cerr<<"Error retrieving dictionary from get_message method"<<std::endl;
            throw 5;
        }
        return message_map;
    }
}


void CAN_Python_Interface::sendMessage(std::map<std::string,std::string> message_map)
{
    if(CAN_Python_Interface::python_can_class == nullptr)
    {
        std::cerr<<"Error, python module not initialized"<<std::endl;
        throw 5;
    }
    else
    {
        PyObject *pValue;
        pValue = PyObject_CallMethod(CAN_Python_Interface::python_can_class, "send_config_message","(iii)",message_map["ABSMode"],message_map["TCMode"], message_map["ThrottleResponseMode"]);

        if (pValue)
            Py_DECREF(pValue);
        else
            PyErr_Print();
    }
}

//method used mainly for testing
bool CAN_Python_Interface::checkClassInstantiated()
{
    if(CAN_Python_Interface::python_can_class == nullptr)
        return false;
    else
        return true;
}