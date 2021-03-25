#include "edgeAI_functions.hpp"
const int CRASH_FUNCTION = 1;

DFLOW_Onboard_Addon_Functions::DFLOW_Onboard_Addon_Functions(int type,std::string model_file) : model(fdeep::load_model(model_file.c_str()))
{
    DFLOW_Onboard_Addon_Functions::type= type;
}


bool DFLOW_Onboard_Addon_Functions::crashAIfunction(OnBoardDataInterface data_interface)
{
    if(DFLOW_Onboard_Addon_Functions::type == CRASH_FUNCTION)
    {
        data_interface.getSignalBatch();
        float lean_angle = data_interface.getFloatData(LEAN_ANGLE_PIPE);
        float accel_x = data_interface.getFloatData(ACCELERATION_X_PIPE);
        float accel_y = data_interface.getFloatData(ACCELERATION_Y_PIPE);
        const auto result = DFLOW_Onboard_Addon_Functions::model.predict(
            {fdeep::tensor(fdeep::tensor_shape(static_cast<std::size_t>(3)),
            std::vector<float>{lean_angle, accel_x, accel_y})});
        const float result_value = fdeep::internal::to_singleton_value(fdeep::internal::single_tensor_from_tensors(result));
        if(result_value>0.6)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}