# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial
import matplotlib.pyplot as plt

from statistics import mean


def read_plot_matrix():
    print("waiting ...")
    n_str = ser.read_until(b"\n")
    print(n_str)
    # get the number of data points to receive
    n_int = int(n_str)  # turn it into an int
    print("Data lengeth = " + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b"\n")
        # get the data as a string, ints seperated by spaces
        dat_int = list(map(int, dat_str.split()))  # now the data is a list
        ref.append(dat_int[0])
        data.append(dat_int[1])
        data_received = data_received + 1
        # print(dat_int[2])
    meanzip = zip(ref, data)
    meanlist = []
    for i, j in meanzip:
        meanlist.append(abs(i - j))
    score = mean(meanlist)
    t = range(len(ref))  # index array
    plt.plot(t, ref, "r-", label="Reference Current")
    plt.plot(t, data, "b-", label="Output Current")
    plt.title("Score = " + str(score))
    plt.ylabel("value")
    plt.xlabel("index")
    plt.legend()
    plt.show()


if __name__ == "__main__":

    ser = serial.Serial("/dev/ttyUSB0", 230400)
    print("Opening port: ")
    print(ser.name)

    has_quit = False
    # menu loop
    while not has_quit:
        print("PIC32 MOTOR DRIVER INTERFACE")

        # display the menu options; this list will grow
        print("\ta: Read current sensor (ADC counts) \tb: Read current sensor (mA)")
        print("\tc: Read encoder (counts) \t\td: Read encoder (deg)")
        print("\te: Reset encoder\t\t\tf: Set PWM (-100 to 100)")
        print("\tg: Set current gains\t\t\th: Get current gains")
        print("\ti: Set position gains\t\t\tj: Get position gains")
        print("\tk: Test current control\t\t\tl: Go to angle (deg)")
        print("\tm: Load set trajectory\t\t\tn: Load cubic trajectory")
        print("\to: Execute trajectory\t\t\tp: Unpower trajectory")
        print("\tq: Quit client\t\t\t\tr: Get mode")

        # read the user's choice
        selection = input("\nENTER COMMAND: ")
        selection_endline = selection + "\n"

        # send the command to the PIC32
        ser.write(selection_endline.encode())
        # .encode() turns the string into a char array

        # take the appropriate action
        # there is no switch() in python, using if elif instead
        if selection == "b":
            print("Requesting Current Reading")
            current_str = ser.read_until(b"\n")
            current = float(current_str.decode())
            print(f"Current reading is {current} mA")

        elif selection == "c":
            print("Requesting Encoder count")
            count_str = ser.read_until(b"\n")
            count = int(count_str.decode())
            print(f"The motor angle is {count} counts")

        elif selection == "d":
            print("Requesting Encoder degree")
            deg_str = ser.read_until(b"\n")
            deg = float(deg_str.decode())
            print(f"The motor angle is {deg} degrees")

        elif selection == "e":
            print("Encoder value has been reset")

        elif selection == "f":
            pwm = input("What PWM value would you like [-100, 100]? ")
            pwm_str = pwm + "\n"
            ser.write(pwm_str.encode())

            speed_str = ser.read_until(b"\n")
            speed = int(speed_str)

            if speed >= 0:
                print(f"PWM has been set to {speed}% in the clockwise direction.")

            else:
                print(
                    f"PWM has been set to {-speed}% in the counterclockwise direction."
                )

        elif selection == "g":
            kp = input("Enter your desired Kp gain [recommanded: 4.76]: ")
            ki = input("Enter your desired Ki gain [recommanded: 0.32]: ")

            kp_str = kp + "\n"
            ki_str = ki + "\n"

            ser.write(kp_str.encode())
            ser.write(ki_str.encode())

        elif selection == "h":
            kp_str = ser.read_until(b"\n")
            ki_str = ser.read_until(b"\n")

            kp = float(kp_str)
            ki = float(ki_str)

            print(f"The current controller is using Kp = {kp}, Ki = {ki}")

        elif selection == "i":
            kp = input("Enter your desired Kp gain [recommanded: 4.76]: ")
            ki = input("Enter your desired Ki gain [recommanded: 0.32]: ")
            kd = input("Enter your desired Kd gain [recommanded: 10.63]: ")

            kp_str = kp + "\n"
            ki_str = ki + "\n"
            kd_str = kd + "\n"

            ser.write(kp_str.encode())
            ser.write(ki_str.encode())
            ser.write(kd_str.encode())

        elif selection == "j":
            kp_str = ser.read_until(b"\n")
            ki_str = ser.read_until(b"\n")
            kd_str = ser.read_until(b"\n")

            kp = float(kp_str)
            ki = float(ki_str)
            kd = float(kd_str)

            print(f"The position controller is using Kp = {kp}, Ki = {ki}, Kd = {kd}")

        elif selection == "k":
            read_plot_matrix()

        elif selection == "l":
            desired_deg = input("Enter the desired motor angle in degrees: ")
            desired_deg_str = f"{desired_deg}\n"

            print(f"Motor moving to {desired_deg} degrees.")
            ser.write(desired_deg_str.encode())

        elif selection == "q":
            print("Exiting client")
            has_quit = True
            # exit client
            # be sure to close the port
            ser.close()

        elif selection == "r":
            state_str = ser.read_until(b"\n").decode()
            print(f"The PIC32 controller is currently {state_str[:-2]}")

        else:
            print("Invalid Selection " + selection_endline)

        print("")
