#include "can_interface.hpp"


CAN_Interface::CAN_Interface(std::string dbc_file_path, std::string python_file_path)
{
    CAN_Interface::dbc_file_path = dbc_file_path;
    CAN_Interface::python_file_path = python_file_path;
}

PyObject* CAN_Interface::instantiatePythonClass()
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
        return nullptr;
    }
    Py_DECREF(module_name);

    // dict is a borrowed reference.
    dict = PyModule_GetDict(module);
    if (dict == nullptr) {
        PyErr_Print();
        std::cout << "Fails to get the dictionary.\n"<<std::endl;
        return nullptr;
    }
    Py_DECREF(module);

    // Builds the name of a callable class
    python_class = PyDict_GetItemString(dict, "CAN_Decoder");
    if (python_class == nullptr) {
        PyErr_Print();
        std::cout << "Fails to get the Python class.\n"<<std::endl;
        return nullptr;
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
        return nullptr;
    }

    return object;
} 

void CAN_Interface::setListener(std::vector<Pipes> output_pipes)
{
    PyObject* python_can_class;
    python_can_class = CAN_Interface::instantiatePythonClass();

    if(python_can_class != nullptr)
    {
        while(true)
        {
            std::map<std::string, std::string> my_map;

            PyObject *pDict = PyObject_CallMethod(python_can_class, "get_message","()"); 

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

                if(my_map["MessageType"]== "IMUSensor")
                {

                    IMUSensorMessage imu_message;
                    imu_message.set_from_map(my_map);
                    write(output_pipes[IMU_MESSAGE_PIPE].rdwr[WRITE], &imu_message.data, sizeof(imu_message.data));
                }
                else if(my_map["MessageType"]== "EngineSensors")
                {
                    EngineSensorsMessage engine_message;
                    engine_message.set_from_map(my_map);
                    write(output_pipes[ENGINE_MESSAGE_PIPE].rdwr[WRITE], &engine_message.data, sizeof(engine_message.data));
                }
                else if(my_map["MessageType"]== "ABSModule")
                {
                    ABSModuleMessage abs_message;
                    abs_message.set_from_map(my_map);
                    write(output_pipes[ABS_MESSAGE_PIPE].rdwr[WRITE], &abs_message.data, sizeof(abs_message.data));
                }
                else if(my_map["MessageType"]== "TPMModule")
                {
                    TPMModuleMessage tpm_message;
                    tpm_message.set_from_map(my_map);
                    write(output_pipes[TPM_MESSAGE_PIPE].rdwr[WRITE], &tpm_message.data, sizeof(tpm_message.data));
                }
                else if(my_map["MessageType"]== "IntakeSensors")
                {
                    IntakeSensorsMessage intake_message;
                    intake_message.set_from_map(my_map);
                    write(output_pipes[INTAKE_MESSAGE_PIPE].rdwr[WRITE], &intake_message.data, sizeof(intake_message.data));
                }
                else if(my_map["MessageType"]== "ConfigurableMode")
                {
                    ConfigurableModesMessage config_message;
                    config_message.set_from_map(my_map);
                    write(output_pipes[CONFIG_MESSAGE_PIPE].rdwr[WRITE], &config_message.data, sizeof(config_message.data));
                }
            }
        }
        Py_Finalize();
    }
    else
    {
        std::cout<<"Error: python interface instatiated incorrectly"<<std::endl;
    }
}

void CAN_Interface::sendConfigMessage(ConfigurableModesMessage message)
{
    // Initializes the Python interpreter
    PyObject* python_can_class;
    python_can_class = CAN_Interface::instantiatePythonClass();

    PyObject *pValue;
    pValue = PyObject_CallMethod(python_can_class, "send_config_message","(iii)",message.data.abs_mode,message.data.tc_mode, message.data.throttle_response_mode);

    if (pValue)
        Py_DECREF(pValue);
    else
        PyErr_Print();
}