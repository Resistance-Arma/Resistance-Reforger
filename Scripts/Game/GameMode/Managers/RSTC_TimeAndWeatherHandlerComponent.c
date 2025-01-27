//------------------------------------------------------------------------------------------------
class RSTC_TimeAndWeatherHandlerComponentClass: SCR_TimeAndWeatherHandlerComponentClass
{
};

//------------------------------------------------------------------------------------------------
class RSTC_TimeAndWeatherHandlerComponent : SCR_TimeAndWeatherHandlerComponent
{
	float GetDayTimeMultiplier()
	{
		return m_fDayTimeAcceleration;
	}
}