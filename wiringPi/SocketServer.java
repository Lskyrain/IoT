import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.ObjectOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

import com.pi4j.wiringpi.Gpio;
import com.pi4j.wiringpi.SoftPwm;
import com.pi4j.io.gpio.GpioController;
import com.pi4j.io.gpio.GpioFactory;
import com.pi4j.io.gpio.GpioPinDigitalOutput;
import com.pi4j.io.gpio.PinState;
import com.pi4j.io.gpio.RaspiPin;
import com.pi4j.io.gpio.*;

public class SocketServer implements Runnable {

	public static final int ServerPort = 9999;
	int cnt=-1, on_off=0, exist;

	ultrasonic sonic = new ultrasonic(03,04,1000,23429411);

	@Override

	public void run(){
		try {
			ServerSocket serverSocket = new ServerSocket(ServerPort);
			System.out.println("Connecting...");

			while (true) {
			if(sonic.getDistance() < 500)
				exist = 1;
			else if(sonic.getDistance() > 500)
				exist = 0;

			if((exist == 0) && (on_off == 1))
			{
				for(int i=0; i<10; i++)
				{
					Thread.sleep(1000);
					cnt++;				
					if(cnt == 5)
					{
						SoftPwm.softPwmWrite(00, 0);
						cnt = 0;
						break;
					}
				}
			}
			Socket client = serverSocket.accept();

			try {                              
				BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
				String str = in.readLine();
				int a = Integer.parseInt(str);			

				Gpio.wiringPiSetup();
				SoftPwm.softPwmCreate(00, 0, 100);

				if(a == 1)
				{
					SoftPwm.softPwmWrite(00, 15);
					on_off = 1;
				}
				else if(a == 2)
				{
					SoftPwm.softPwmWrite(00, 50);
					on_off = 1;
				}
				else if(a == 3)
				{
					SoftPwm.softPwmWrite(00, 100);
					on_off = 1;
				}
				else if(a == 0)
				{
					SoftPwm.softPwmWrite(00, 0);
					on_off = 0;
				}

				System.out.println("distance" + sonic.getDistance() + "mm");

				System.out.println("Received: '" + str + "'");
				PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(client.getOutputStream())), true);
				out.println("Server Received : '" + str + "'");
				} catch (Exception e) {
					System.out.println("Error");
					e.printStackTrace();
				} finally {
					client.close();
					System.out.println("");
				}
			}
			} catch (Exception e) {
			System.out.println("S: Error");
			e.printStackTrace();
    			    }
			}
	public static void main(String[] args) throws InterruptedException {
		Thread ServerThread = new Thread(new SocketServer());
		ServerThread.start();
	}

	class ultrasonic{

		//bcm, GPIO_#

		private int PIN_ECHO,PIN_TRIG;

		private long REJECTION_START=1000,REJECTION_TIME=1000; //ns

 

		private GpioController gpio;//gpio controller ; using io.gpio

		private GpioPinDigitalOutput//gpio output pins; using io.gpio, digital pins

			pin_trig;

		private GpioPinDigitalInput

			pin_echo;

		public ultrasonic(int ECHO, int TRIG, long REJ_START,long REJ_TIME){ //GPIO
			this.PIN_ECHO=ECHO;
			this.PIN_TRIG=TRIG;
			this.REJECTION_START=REJ_START; this.REJECTION_TIME=REJ_TIME;

			gpio=GpioFactory.getInstance();// create gpio controller , io.gpio

		//motor_1_left_en=gpio.provisionDigitalOutputPin(RaspiPin.GPIO_01, "motor_1_left_en", PinState.HIGH);

			pin_trig=gpio.provisionDigitalOutputPin(RaspiPin.getPinByAddress(PIN_TRIG), "pin_trig", PinState.HIGH);//pin,tag,initial-state

			pin_trig.setShutdownOptions(true, PinState.LOW);


			pin_echo=gpio.provisionDigitalInputPin(RaspiPin.getPinByAddress(PIN_ECHO),PinPullResistance.PULL_DOWN);//pin,tag,initial-state

		}

		public int getDistance() throws Exception{ //in milimeters

			int distance=0; long start_time, end_time,rejection_start=0,rejection_time=0;
			//Start ranging- trig should be in high state for 10us to generate ultrasonic signal
			//this will generate 8 cycle sonic burst.
			// produced signal would looks like, _|-----|
			pin_trig.low(); busyWaitMicros(2);
			pin_trig.high(); busyWaitMicros(10);
			pin_trig.low();

			//echo pin high time is propotional to the distance _|----|
			//distance calculation
			while(pin_echo.isLow()){ //wait until echo get high
				busyWaitNanos(1); //wait 1ns
				rejection_start++;
				if(rejection_start==REJECTION_START) return -1; //infinity
			}

			start_time=System.nanoTime();

			while(pin_echo.isHigh()){ //wait until echo get low
				busyWaitNanos(1); //wait 1ns
				rejection_time++;
				if(rejection_time==REJECTION_TIME) return -1; //infinity
			}

			end_time=System.nanoTime();
			distance=(int)((end_time-start_time)/5882.35294118); //distance in mm
			//distance=(end_time-start_time)/(200*29.1); //distance in mm
			return distance;
	}

	public void busyWaitMicros(long micros){
		long waitUntil = System.nanoTime() + (micros * 1_000);
		while(waitUntil > System.nanoTime()){;}
	}

   	 public void busyWaitNanos(long nanos){
		long waitUntil = System.nanoTime() + nanos;
		while(waitUntil > System.nanoTime()){;}
	}

	}
}
