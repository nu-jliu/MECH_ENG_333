# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial

ser = serial.Serial("/dev/ttyUSB0", 230400)
print("Opening port: ")
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print("PIC32 MOTOR DRIVER INTERFACE")
    # display the menu options; this list will grow
    print("\td: Dummy Command \tq: Quit")  # '\t' is a tab
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

    # elif selection == 'e':

    elif selection == "f":
        pwm = input("What PWM value would you like [-100, 100]? ")
        pwm_str = pwm + "\n"
        ser.write(pwm_str.encode())

        speed_str = ser.read_until(b"\n")
        speed = int(speed_str)

        if speed >= 0:
            print(f"PWM has been set to {speed}% in the clockwise direction.")

        else:
            print(f"PWM has been set to {-speed}% in the counterclockwise direction.")

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

    elif selection == "q":
        print("Exiting client")
        has_quit = True
        # exit client
        # be sure to close the port
        ser.close()

    else:
        print("Invalid Selection " + selection_endline)

    print("")
