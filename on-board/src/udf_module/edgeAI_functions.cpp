#include "edgeAI_functions.hpp"
const int CRASH_FUNCTION = 1;
const int FLOAT_UDF_DATA_TYPE = 0;
const int INT_UDF_DATA_TYPE = 1;
const int BOOL_UDF_DATA_TYPE = 2;

DFLOW_Onboard_Addon_Functions::DFLOW_Onboard_Addon_Functions()
{}


float DFLOW_Onboard_Addon_Functions::AIfunction(OnBoardDataInterface data_interface,std::vector<int> desired_data,std::vector<int> desired_data_types, std::string model_file)
{
    fdeep::model model = fdeep::load_model(model_file.c_str());

    std::vector<int> integer_values;
    std::vector<float> float_values;
    std::vector<bool> bool_values;
    //get the signal batch and the required data
    data_interface.getSignalBatch();

    for(int i = 0; i < desired_data.size(); i++)
    {
        if(desired_data_types[i] == FLOAT_UDF_DATA_TYPE)
        {
            float_values.push_back(data_interface.getFloatData(desired_data[i]));
        }
        else if(desired_data_types[i] == INT_UDF_DATA_TYPE)
        {
            integer_values.push_back(data_interface.getIntegerData(desired_data[i]));
        }
        else if(desired_data_types[i] == BOOL_UDF_DATA_TYPE)
        {
            bool_values.push_back(data_interface.getBooleanData(desired_data[i]));
        }
    }
    for(int i =0; i < integer_values.size(); i++)
    {
        float_values.push_back(static_cast<float>(integer_values[i]));
    }
    for(int i =0; i < bool_values.size(); i++)
    {
        float_values.push_back(static_cast<float>(bool_values[i]));
    }

    //get the model prediction
    const auto result = model.predict(
        {fdeep::tensor(fdeep::tensor_shape(static_cast<std::size_t>(3)),
        float_values)});
    //convert prediction from tensor to float
    const float result_value = fdeep::internal::to_singleton_value(fdeep::internal::single_tensor_from_tensors(result));
    //classify the regression result(sigmoid function is used so no need for accuracy)
    return result_value;
}