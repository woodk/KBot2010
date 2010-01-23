#include "DashboardDataSender.h"
#include "Timer.h"

/**
 * Send data to the dashboard.
 * This class sends two types of data to the dashboard program:
 * 1. Data representing all the ports on the robot
 * 2. Camera tracking data so the dashboard can annotate the video stream with
 *    target information.
 */
DashboardDataSender::DashboardDataSender()
{
	// these timers make sure that the data is not sent to the dashboard more
	// than 10 times per second for efficiency.
	IOTimer = new Timer();
	visionTimer = new Timer();
	IOTimer->Start();
	visionTimer->Start();
}

/**
 * Send the vision tracking data.
 * Sends the vision information to the dashboard so that the images will be annotated
 * and the graphs will operate.
 */
void DashboardDataSender::sendVisionData(double joyStickX,
					double gyroAngle,
					double gyroRate,
					double targetX,
					vector<Target> targets) {
	if (visionTimer->Get() < 0.1)
		return;
	visionTimer->Reset();
	Dashboard &dash = DriverStation::GetInstance()->GetHighPriorityDashboardPacker();
	dash.AddCluster(); // wire (2 elements)
	{
		dash.AddCluster(); // tracking data
		{
			dash.AddDouble(joyStickX); // Joystick X
			dash.AddDouble(((((int)gyroAngle) + 360 + 180) % 360) - 180.0); // angle
			dash.AddDouble(0.0); // angular rate
			dash.AddDouble(targetX); // other X
		}
		dash.FinalizeCluster();
		dash.AddCluster(); // target Info (2 elements)
		{
			dash.AddArray(); // targets
			{
                for (unsigned i = 0; i < targets.size(); i++) {
                    dash.AddCluster(); // targets
                    {
                        dash.AddDouble(targets[i].m_score); // target score
                        dash.AddCluster(); // Circle Description (5 elements)
                        {
                            dash.AddCluster(); // Position (2 elements)
                            {
                                dash.AddFloat((float) (targets[i].m_xPos / targets[i].m_xMax)); // X
                                dash.AddFloat((float) targets[i].m_yPos); // Y
                            }
                            dash.FinalizeCluster();

                            dash.AddDouble(targets[i].m_rotation); // Angle
                            dash.AddDouble(targets[i].m_majorRadius); // Major Radius
                            dash.AddDouble(targets[i].m_minorRadius); // Minor Radius
                            dash.AddDouble(targets[i].m_rawScore); // Raw score
                            }
                        dash.FinalizeCluster(); // Position
                        }
                    dash.FinalizeCluster(); // targets
                    }
			}
			dash.FinalizeArray();
			
			dash.AddU32((int) 0);

		}
		dash.FinalizeCluster(); // target Info
	}
	dash.FinalizeCluster(); // wire
	dash.Finalize();
}

/**
 * Send IO port data to the dashboard.
 * Send data representing the output of all the IO ports on the cRIO to the dashboard.
 * This is probably not the best data to send for your robot. Better would be higher
 * level information like arm angle or collector status. But this is a sample and you're
 * free to modify it. Be sure to make the corresponding changes in the LabVIEW example
 * dashboard program running on your driver station.
 */
void DashboardDataSender::sendIOPortData() {
	if (IOTimer->Get() < 0.1)
		return;
	IOTimer->Reset();
	Dashboard &dash = DriverStation::GetInstance()->GetLowPriorityDashboardPacker();
	dash.AddCluster();
	{
		dash.AddCluster();
		{ //analog modules 
			dash.AddCluster();
			{
				for (int i = 1; i <= 8; i++) {
//					dash.AddFloat((float) AnalogModule::GetInstance(1)->GetAverageVoltage(i));
					dash.AddFloat((float) i * 5.0 / 8.0);
				}
			}
			dash.FinalizeCluster();
			dash.AddCluster();
			{
				for (int i = 1; i <= 8; i++) {
					dash.AddFloat((float) AnalogModule::GetInstance(2)->GetAverageVoltage(i));
				}
			}
			dash.FinalizeCluster();
		}
		dash.FinalizeCluster();

		dash.AddCluster();
		{ //digital modules
			dash.AddCluster();
			{
				dash.AddCluster();
				{
					int module = 4;
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayReverse());
					//					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIO());
					dash.AddU16((short) 0xAAAA);
					//					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIODirection());
					dash.AddU16((short) 0x7777);
					dash.AddCluster();
					{
						for (int i = 1; i <= 10; i++) {
							//							dash.AddU8((unsigned char) DigitalModule::GetInstance(module)->GetPWM(i));
							dash.AddU8((unsigned char) (i-1) * 255 / 9);
						}
					}
					dash.FinalizeCluster();
				}
				dash.FinalizeCluster();
			}
			dash.FinalizeCluster();

			dash.AddCluster();
			{
				dash.AddCluster();
				{
					int module = 6;
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIO());
					dash.AddU16(DigitalModule::GetInstance(module)->GetDIODirection());
					dash.AddCluster();
					{
						for (int i = 1; i <= 10; i++) {
							//							dash.AddU8((unsigned char) DigitalModule::GetInstance(module)->GetPWM(i));
							dash.AddU8((unsigned char) i * 255 / 10);
						}
					}
					dash.FinalizeCluster();
				}
				dash.FinalizeCluster();
			}
			dash.FinalizeCluster();
		}
		dash.FinalizeCluster();

		// Can't read solenoids without an instance of the object
		dash.AddU8((char) 0);
	}
	dash.FinalizeCluster();
	dash.Finalize();
}
