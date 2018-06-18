#!/usr/bin/python
import tkinter as tk
import time
import linecache


def onGo():
    while (on_hit):
        linecache.updatecache('/home/ESL/Software/software_package/in4073/pc_terminal/data_print.txt')
        last_line =linecache.getlines('/home/ESL/Software/software_package/in4073/pc_terminal/data_print.txt')
        line_size = len(last_line)
        last_line2 = linecache.getline('/home/ESL/Software/software_package/in4073/pc_terminal/data_print.txt',line_size-1)
        data_list = last_line2.split()
        list_size = len(data_list)
        #data_last = data_list[list_size-1]
        if list_size == 22:
             data_mode = data_list[0]
             data_theta = data_list[7]
             #data1 = data_list[9]
             #data2 = data_list[10]
             #data3 = data_list[11]
             e_mode.delete(0, tk.END)
             e_battery.delete(0, tk.END)
             e_ae1.delete(0, tk.END)
             e_ae2.delete(0, tk.END)
             e_ae3.delete(0, tk.END)
             e_ae4.delete(0, tk.END)
             e_theta.delete(0, tk.END)
             e_phi.delete(0, tk.END)
             e_psi.delete(0,tk.END)
             e_gyr1.delete(0, tk.END)
             e_gyr2.delete(0, tk.END)
             e_gyr3.delete(0, tk.END)
             e_acc1.delete(0, tk.END)
             e_acc2.delete(0, tk.END)
             e_acc3.delete(0, tk.END)
             e_lift.delete(0,tk.END)
             e_roll.delete(0,tk.END)
             e_pitch.delete(0,tk.END)
             e_yaw.delete(0,tk.END)
             e_p1.delete(0,tk.END)
             e_p2.delete(0,tk.END)
             e_p3.delete(0,tk.END)
             e_mode.insert(1,data_mode)
             e_battery.insert(1,data_list[5])
             e_ae1.insert(1,data_list[1])
             e_ae2.insert(1, data_list[2])
             e_ae3.insert(1, data_list[3])
             e_ae4.insert(1, data_list[4])
             e_theta.insert(1, data_theta)
             e_phi.insert(1, data_list[6])
             e_psi.insert(1, data_list[8])
             e_gyr1.insert(1, data_list[9])
             e_gyr2.insert(1, data_list[10])
             e_gyr3.insert(1, data_list[11])
             e_acc1.insert(1, data_list[12])
             e_acc2.insert(1, data_list[13])
             e_acc3.insert(1, data_list[14])
             e_lift.insert(1, data_list[15])
             e_roll.insert(1, data_list[16])
             e_pitch.insert(1, data_list[17])
             e_yaw.insert(1, data_list[18])
             e_p1.insert(1, data_list[19])
             e_p2.insert(1, data_list[20])
             e_p3.insert(1, data_list[21])
             e_mode.update()
             e_battery.update()
             e_ae1.update()
             e_ae2.update()
             e_ae3.update()
             e_ae4.update()
             e_theta.update()
             e_phi.update()
             e_psi.update()
             e_gyr1.update()
             e_gyr2.update()
             e_gyr3.update()
             e_acc1.update()
             e_acc2.update()
             e_acc3.update()
             e_lift.update()
             e_roll.update()
             e_pitch.update()
             e_yaw.update()
             e_p1.update()
             e_p2.update()
             e_p3.update()
             time.sleep(0.05)
             linecache.clearcache()

def stop():
    global on_hit
    on_hit = False
    onGo()

def start():
    global on_hit
    on_hit = True
    onGo()

window = tk.Tk()
window.title('Copter')
window.geometry('700x700')
l_space0 = tk.Label(window)
l_space0.grid(row=0)
p = tk.PanedWindow(window, orient='vertical')
f_mode = tk.LabelFrame(window, text='mode', width=698, height=50)
f_motor = tk.LabelFrame(window, text='motor', width=698, height=100)
f_battery = tk.LabelFrame(window, text='battery', width=698, height=50)
f_angle = tk.LabelFrame(window, text='pji,theta,psi', width=698, height=100)
f_acc = tk.LabelFrame(window, text='accelerometer', width=698, height=100)
f_gyr = tk.LabelFrame(window, text='gyroscope', width=698, height=100)
f_signal = tk.LabelFrame(window, text='lift, roll, pitch, yaw', width=698, height=100)
f_controller = tk.LabelFrame(window, text='P, P1, P2', width=698, height=100)
p.add(f_mode)
p.add(f_battery)
p.add(f_signal)
p.add(f_motor)
p.add(f_angle)
p.add(f_acc)
p.add(f_gyr)
p.add(f_controller)

p.grid(row =0 ,column=0)

#mode

e_mode = tk.Entry(f_mode)
e_mode.grid(row=1, column=2)
l_mode = tk.Label(f_mode, text='mode :')
l_mode.grid(row = 1, column=1)
l_space_motor1 = tk.Label(f_motor)
l_space_motor1.grid(row=0)
l_space_motor2 = tk.Label(f_motor)
l_space_motor2.grid(row=2)
#battery
e_battery = tk.Entry(f_battery)
e_battery.grid(row=1, column=2)
l_battery = tk.Label(f_battery, text='battery :')
l_battery.grid(row = 1, column=1)
l_space1_battery1 = tk.Label(f_battery)
l_space1_battery1.grid(row=0)
l_space1_battery2 = tk.Label(f_battery)
l_space1_battery2.grid(row=2)
#motor
e_ae1 = tk.Entry(f_motor)
e_ae1.grid(row=1, column=2)
e_ae2 = tk.Entry(f_motor)
e_ae2.grid(row=1, column=4)
e_ae3 = tk.Entry(f_motor)
e_ae3.grid(row=2, column=2)
e_ae4 = tk.Entry(f_motor)
e_ae4.grid(row=2, column=4)
l_ae1 = tk.Label(f_motor, text='ae[0] :')
l_ae1.grid(row = 1, column=1)
l_ae2 = tk.Label(f_motor, text='ae[1] :')
l_ae2.grid(row = 1, column=3)
l_ae3 = tk.Label(f_motor, text='ae[2] :')
l_ae3.grid(row = 2, column =1)
l_ae4 = tk.Label(f_motor, text='ae[3] :')
l_ae4.grid(row = 2, column =3)
l_space_motor = tk.Label(f_motor)
l_space_motor.grid(row=0)
l_space2_motor = tk.Label(f_motor)
l_space2_motor.grid(row=3)

#theta phi psi
e_theta = tk.Entry(f_angle)
e_theta.grid(row=1, column=2)
e_phi = tk.Entry(f_angle)
e_phi.grid(row=1, column=4)
e_psi = tk.Entry(f_angle)
e_psi.grid(row=1, column=6)
#var = tk.StringVar()
l_theta = tk.Label(f_angle, text='theta :')
l_theta.grid(row = 1, column=1)
l_phi = tk.Label(f_angle, text='phi :')
l_phi.grid(row = 1, column=3)
l_psi = tk.Label(f_angle, text='psi :')
l_psi.grid(row = 1, column =5)
l_space1 = tk.Label(f_angle)
l_space1.grid(row=0)
l_space2 = tk.Label(f_angle)
l_space2.grid(row=2)

#accelerometer
e_acc1 = tk.Entry(f_acc)
e_acc1.grid(row=1, column=2)
e_acc2 = tk.Entry(f_acc)
e_acc2.grid(row=1, column=4)
e_acc3 = tk.Entry(f_acc)
e_acc3.grid(row=1, column=6)
#var = tk.StringVar()
l_acc1 = tk.Label(f_acc, text='sax :')
l_acc1.grid(row = 1, column=1)
l_acc2 = tk.Label(f_acc, text='say :')
l_acc2.grid(row = 1, column=3)
l_acc3 = tk.Label(f_acc, text='saz :')
l_acc3.grid(row = 1, column =5)
l_space1 = tk.Label(f_acc)
l_space1.grid(row=0)
l_space2 = tk.Label(f_acc)
l_space2.grid(row=2)

#gyroscope
l_space3 = tk.Label(f_gyr)
l_space3.grid(row=3)
e_gyr1 = tk.Entry(f_gyr)
e_gyr1.grid(row=4, column=2)
e_gyr2 = tk.Entry(f_gyr)
e_gyr2.grid(row=4, column=4)
e_gyr3 = tk.Entry(f_gyr)
e_gyr3.grid(row=4, column=6)
#var = tk.StringVar()
l_gyr1 = tk.Label(f_gyr, text='sp :')
l_gyr1.grid(row = 4, column=1)
l_gyr2 = tk.Label(f_gyr, text='sq :')
l_gyr2.grid(row = 4, column=3)
l_gyr3 = tk.Label(f_gyr, text='sr :')
l_gyr3.grid(row = 4, column=5)
l_space3 = tk.Label(f_gyr)
l_space3.grid(row=5)

#lift, roll ,pitch, yaw
e_lift = tk.Entry(f_signal)
e_lift.grid(row=1, column=2)
e_roll = tk.Entry(f_signal)
e_roll.grid(row=1, column=4)
e_pitch = tk.Entry(f_signal)
e_pitch.grid(row=2, column=2)
e_yaw = tk.Entry(f_signal)
e_yaw.grid(row=2, column=4)
l_lift = tk.Label(f_signal, text='lift :')
l_lift.grid(row = 1, column=1)
l_roll = tk.Label(f_signal, text='roll :')
l_roll.grid(row = 1, column=3)
l_pitch = tk.Label(f_signal, text='pitch :')
l_pitch.grid(row = 2, column =1)
l_yaw = tk.Label(f_signal, text='yaw :')
l_yaw.grid(row = 2, column =3)
l_space_motor = tk.Label(f_signal)
l_space_motor.grid(row=0)
l_space2_motor = tk.Label(f_signal)
l_space2_motor.grid(row=3)

#P,P1,P2
e_p1 = tk.Entry(f_controller)
e_p1.grid(row=1, column=2)
e_p2 = tk.Entry(f_controller)
e_p2.grid(row=1, column=4)
e_p3 = tk.Entry(f_controller)
e_p3.grid(row=1, column=6)
l_p1 = tk.Label(f_controller, text='P :')
l_p1.grid(row = 1, column=1)
l_p2 = tk.Label(f_controller, text='P1 :')
l_p2.grid(row = 1, column=3)
l_p3 = tk.Label(f_controller, text='P2 :')
l_p3.grid(row = 1, column =5)
l_space1 = tk.Label(f_controller)
l_space1.grid(row=0)
l_space2 = tk.Label(f_controller)
l_space2.grid(row=2)




goBtn1=tk.Button(text='start', command = start)
goBtn1.grid(row=6)

goBtn2=tk.Button(text='stop', command = stop)
goBtn2.grid(row=7)
window.mainloop()
