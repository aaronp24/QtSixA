#!/usr/bin/env python
# -*- coding: utf-8 -*-

import commands, dbus, os, sys, time
from functools import partial
from PyQt4 import QtCore, QtGui, uic

#For easy debugging, print version information to terminal
print "Main Qt version:", QtCore.QT_VERSION_STR
print "Python-Qt version:", QtCore.PYQT_VERSION_STR

#Create configuration files if they don't exist
os.system("if [ -d $HOME/.config/autostart ]; then true; else mkdir -p $HOME/.config; mkdir -p $HOME/.config/autostart; fi")
os.system("if [ -f $HOME/.qtsixa/qtsixa.conf ]; then true; else mkdir -p $HOME/.qtsixa; cp /usr/share/qtsixa/qtsixa.conf.bu $HOME/.qtsixa/qtsixa.conf; fi")

#List Of Sixaxis profiles
listOfSixaxisProfiles = open('/usr/share/qtsixa/profiles.list').read().split()

#Read preferences file
config_file = commands.getoutput('cat "$HOME/.qtsixa/qtsixa.conf"').split()
config_enable_systray = config_file[1]
config_start_minimized = config_file[3]
config_close_to_tray = config_file[5]
config_display_info = config_file[7]

#Use dbus for disconnect devices (no root required)
bus = dbus.SystemBus()

#Check for root tool
ROOT = commands.getoutput("sixa "+"get-root")
print "Will use '"+ROOT.split()[0]+"' for root actions"


def look4Root():
    if not "kdesudo" in ROOT and "kdesu" in ROOT: #Fix for openSUSE's kdesu not echoing to terminal (opens separate session for sudo)
	return 1
    elif "YES" in commands.getoutput(ROOT+" echo YES"):
	return 1
    else:
        QtGui.QMessageBox.critical(QtSixA, "QtSixA - Error", "Operation not permitted - Not enough rights")
	return 0


#-----------
# About Window
#---------------------------------------
class QtSixA_About_Window(QtGui.QDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)
       
#Set icon and uic file
        self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
        uic.loadUi("/usr/share/qtsixa/gui/qtsixa_about.ui",  self)


#---------------
# Reference pop-up
#-------------------------------------
class QtSixA_Reference(QtGui.QDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)
       
#Set icon and uic file
        self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
        uic.loadUi("/usr/share/qtsixa/gui/qtsixa_reference.ui",  self)


#---------------------
# List of available keys
#---------------------------------------
class QtSixA_Profile_PKE(QtGui.QDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)

#Set icon and uic file
	self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
	uic.loadUi("/usr/share/qtsixa/gui/qtsixa_keylist.ui", self)



#--------------------------
# Add new profile to the list
#--------------------------------------
class QtSixA_Add_Window(QtGui.QDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)
       
#Set icon and uic file
        self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
        uic.loadUi("/usr/share/qtsixa/gui/qtsixa_addprofile.ui",  self)
 
	self.connect(self.b_cancel, QtCore.SIGNAL('clicked()'), QtCore.SLOT('close()'))
	self.connect(self.b_loc, QtCore.SIGNAL('clicked()'), self.func_Location)
	self.connect(self.b_png, QtCore.SIGNAL('clicked()'), self.func_PNG_file)
	self.connect(self.b_add, QtCore.SIGNAL('clicked()'), self.func_Add)

    def func_Add(self):
	if (self.line_loc.displayText() == ""):
	    self.func_Warning()
	elif (self.line_png.displayText() == ""):
	    self.func_Warning()
	elif (self.line_short.displayText() == ""):
	    self.func_Warning()
	elif (self.line_full.displayText() == ""):
	    self.func_Warning()
	else:
	    if look4Root():
		os.system(ROOT+" cp "+str(self.location)+" "+"/usr/share/qtsixa/sixaxis-profiles/sixa_"+str(self.line_short.displayText())+".fdi")
		os.system(ROOT+" cp "+str(self.png_file)+" "+"/usr/share/qtsixa/pics/sixa_"+str(self.line_short.displayText())+".png")
		self.profileFileW = open(("/tmp/qtsixa.profile"), "w")
		self.profileFileW.write(str(self.line_short.displayText())+"\t"+str(self.line_full.displayText())+"\n")
		self.profileFileW.close()
		os.system(ROOT+' sudo su root -c \"cat /tmp/qtsixa.profile >> /usr/share/qtsixa/profiles.list\"')
		QtGui.QMessageBox.information(self, self.tr("QtSixA - Done!"), self.tr("Your custom profile "+self.line_full.displayText()+" has been added.\nRestart QtSixA to see changes."))
		QtSixA_Add_Window().close()

    def func_Location(self):
	self.location = QtGui.QFileDialog.getOpenFileName(self, "Open FDI File", "", "QtSixA FDI Files (*fdi *FDI *Fdi)")
	self.line_loc.setText(str(self.location))

    def func_PNG_file(self):
	self.png_file = QtGui.QFileDialog.getOpenFileName(self, "Open PNG File", "", "PNG Image Files (*png *PNG *Png *PnG)")
	self.line_png.setText(str(self.png_file))

    def func_Warning(self):
	QtGui.QMessageBox.warning(self, self.tr("QtSixA - Incomplete!"), self.tr("Something is missing...\n(Empty slot is not possible!)"))




#-------------------
# Create a new profile
#---------------------------------------
class QtSixA_Profile_New(QtGui.QDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)

#Set icon and uic file
	self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
	uic.loadUi("/usr/share/qtsixa/gui/qtsixa_newprofile.ui", self)

	self.connect(self.b_keys, QtCore.SIGNAL('clicked()'), self.func_PKE)
	self.connect(self.b_tips, QtCore.SIGNAL('clicked()'), self.func_Tips)
	self.connect(self.b_cancel, QtCore.SIGNAL('clicked()'), QtCore.SLOT('close()'))
	self.connect(self.b_done, QtCore.SIGNAL('clicked()'), self.func_Done)
	self.connect(self.combo_left, QtCore.SIGNAL('currentIndexChanged(QString)'), self.func_UpdateComboLeft)
	self.connect(self.combo_right, QtCore.SIGNAL('currentIndexChanged(QString)'), self.func_UpdateComboRight)

	self.group_left_h.setEnabled(0)
	self.group_left_v.setEnabled(0)
	self.group_right_h.setEnabled(0)
	self.group_right_v.setEnabled(0)

    def func_PKE(self):
        QtSixA_Profile_PKE().exec_()

    def func_Tips(self):
	QtGui.QMessageBox.information(self, self.tr("QtSixA - Tips & Tricks"), self.tr(""
	"<body style=\"font-size:10pt;\">Some tips you may need:<p><body style=\"font-size:8pt;\">"
	"<b>1. </b>Click on the \"View available keys\" buttons to see a full list of available keys you can assign to a button;<br>"
	"<b>2. </b>Write the key you want to a button input-text;<br>"
	"<b>3. </b>If you don't want a button to work as key set the input-text to \"none\" (without quotes);<br>"
	"<b>4. </b>Don't forget about the 'Axis' part!<br>"
	"<b>5. </b>Combinations are possible using ',' between key names (maximum 4 keys)<br>"
	"<b>6. </b>It's also possible to assign a mouse-button to joystick-button, use \"mouse_BUTTON\" on those you want that. (BUTTON is a number)<br>"
	"<b>7. </b>Once you finish, you can open the generated file for a further customization."))

    def func_UpdateComboLeft(self):
	if (self.combo_left.currentText() == "Custom Buttons"):
	  self.group_left_h.setEnabled(1)
	  self.group_left_v.setEnabled(1)
	else:
	  self.group_left_h.setEnabled(0)
	  self.group_left_v.setEnabled(0)

    def func_UpdateComboRight(self):
	if (self.combo_right.currentText() == "Custom Buttons"):
	  self.group_right_h.setEnabled(1)
	  self.group_right_v.setEnabled(1)
	else:
	  self.group_right_h.setEnabled(0)
	  self.group_right_v.setEnabled(0)

    def func_Warning(self):
	QtGui.QMessageBox.warning(self, self.tr("QtSixA - Incomplete!"), self.tr("Something is missing...\n(Empty slot is not possible!)"))

    def func_Done(self):
	if (self.line_app.displayText() == ""):
	    self.func_Warning()
	elif (self.line_author.displayText() == ""):
	    self.func_Warning()
	elif (self.line_l2.displayText() == ""):
	    self.func_Warning()
	elif (self.line_l1.displayText() == ""):
	    self.func_Warning()
	elif (self.line_r2.displayText() == ""):
	    self.func_Warning()
	elif (self.line_r1.displayText() == ""):
	    self.func_Warning()
	elif (self.line_select.displayText() == ""):
	    self.func_Warning()
	elif (self.line_start.displayText() == ""):
	    self.func_Warning()
	elif (self.line_up.displayText() == ""):
	    self.func_Warning()
	elif (self.line_down.displayText() == ""):
	    self.func_Warning()
	elif (self.line_left.displayText() == ""):
	    self.func_Warning()
	elif (self.line_right.displayText() == ""):
	    self.func_Warning()
	elif (self.line_square.displayText() == ""):
	    self.func_Warning()
	elif (self.line_triangle.displayText() == ""):
	    self.func_Warning()
	elif (self.line_circle.displayText() == ""):
	    self.func_Warning()
	elif (self.line_cross.displayText() == ""):
	    self.func_Warning()

	elif (self.combo_left.currentText() == "Custom Buttons"):
	  if (self.line_hls_left.displayText() == ""):
	    self.func_Warning()
	  elif (self.line_hls_right.displayText() == ""):
	    self.func_Warning()
	  elif (self.line_vls_up.displayText() == ""):
	    self.func_Warning()
	  elif (self.line_vls_down.displayText() == ""):
	    self.func_Warning()
	  elif (self.combo_right.currentText() == "Custom Buttons"):
	    if (self.line_hrs_left.displayText() == ""):
	      self.func_Warning()
	    elif (self.line_hrs_right.displayText() == ""):
	      self.func_Warning()
	    elif (self.line_vrs_up.displayText() == ""):
	      self.func_Warning()
	    elif (self.line_vrs_down.displayText() == ""):
	      self.func_Warning()
	    else:
	      self.func_ExportToFDI()
	  else:
	    self.func_ExportToFDI()

	elif (self.combo_right.currentText() == "Custom Buttons"):
	  if (self.line_hrs_left.displayText() == ""):
	    self.func_Warning()
	  elif (self.line_hrs_right.displayText() == ""):
	    self.func_Warning()
	  elif (self.line_vrs_up.displayText() == ""):
	    self.func_Warning()
	  elif (self.line_vrs_down.displayText() == ""):
	    self.func_Warning()
	  elif (self.combo_left.currentText() == "Custom Buttons"):
	    if (self.line_hls_left.displayText() == ""):
	      self.func_Warning()
	    elif (self.line_hls_right.displayText() == ""):
	      self.func_Warning()
	    elif (self.line_vls_up.displayText() == ""):
	      self.func_Warning()
	    elif (self.line_vls_down.displayText() == ""):
	      self.func_Warning()
	    else:
	      self.func_ExportToFDI()
	  else:
	    self.func_ExportToFDI()

	else:
	  self.func_ExportToFDI()

    def func_ExportToFDI(self):
	if (self.combo_left.currentText() == "None"):
	  self.ComboLeftText_H = "mode=none"
	  self.ComboLeftText_V = "mode=none"
	elif (self.combo_left.currentText() == "Mouse"):
	  self.ComboLeftText_H = "mode=relative axis=+2x deadzone=5000"
	  self.ComboLeftText_V = "mode=relative axis=+2y deadzone=5000"
	elif (self.combo_left.currentText() == "Scroll"):
	  self.ComboLeftText_H = "mode=relative axis=+1zx deadzone=7500"
	  self.ComboLeftText_V = "mode=relative axis=+1zy deadzone=7500"
	elif (self.combo_left.currentText() == "Custom Buttons"):
	  self.ComboLeftText_H = "mode=accelerated keylow="+self.line_hls_left.text()+" keyhigh="+self.line_hls_right.text()+" deadzone=15000"
	  self.ComboLeftText_V = "mode=accelerated keylow="+self.line_vls_up.text()+" keyhigh="+self.line_vls_down.text()+" deadzone=15000"

	if (self.combo_right.currentText() == "None"):
	  self.ComboRightText_H = "mode=none"
	  self.ComboRightText_V = "mode=none"
	elif (self.combo_right.currentText() == "Mouse"):
	  self.ComboRightText_H = "mode=relative axis=+2x deadzone=5000"
	  self.ComboRightText_V = "mode=relative axis=+2y deadzone=5000"
	elif (self.combo_right.currentText() == "Scroll"):
	  self.ComboRightText_H = "mode=relative axis=+1zx deadzone=7500"
	  self.ComboRightText_V = "mode=relative axis=+1zy deadzone=7500"
	elif (self.combo_right.currentText() == "Custom Buttons"):
	  self.ComboRightText_H = "mode=accelerated keylow="+self.line_hrs_left.text()+" keyhigh="+self.line_hrs_right.text()+" deadzone=7500"
	  self.ComboRightText_V = "mode=accelerated keylow="+self.line_vrs_up.text()+" keyhigh="+self.line_vrs_down.text()+" deadzone=7500"

	self.FDI_Content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n<deviceinfo version=\"0.2\">\n  <device>\n    <match key=\"info.capabilities\" contains=\"input\">\n      <!-- Sixaxis configuration for \""+self.line_app.text()+"\", by \""+self.line_author.text()+"\" -->\n      <match key=\"input.product\" contains=\"PLAYSTATION(R)3 Controller\">\n        <merge key=\"input.x11_driver\" type=\"string\">joystick</merge>\n\n        <merge key=\"input.x11_options.MapAxis1\" type=\"string\">"+self.ComboLeftText_H+"</merge>\n        <merge key=\"input.x11_options.MapAxis2\" type=\"string\">"+self.ComboLeftText_V+"</merge>\n        <merge key=\"input.x11_options.MapAxis3\" type=\"string\">"+self.ComboRightText_H+"</merge>\n        <merge key=\"input.x11_options.MapAxis4\" type=\"string\">"+self.ComboRightText_V+"</merge>\n	<merge key=\"input.x11_options.MapAxis5\" type=\"string\">mode=none</merge>\n	<merge key=\"input.x11_options.MapAxis6\" type=\"string\">mode=none</merge>\n	<merge key=\"input.x11_options.MapAxis7\" type=\"string\">mode=none</merge>\n\n        <merge key=\"input.x11_options.MapButton1\" type=\"string\">key="+self.line_select.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton2\" type=\"string\">key=NoSymbol</merge>\n        <merge key=\"input.x11_options.MapButton3\" type=\"string\">key=NoSymbol</merge>\n        <merge key=\"input.x11_options.MapButton4\" type=\"string\">key="+self.line_start.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton17\" type=\"string\">key="+self.line_ps.text()+"</merge>\n\n        <merge key=\"input.x11_options.MapButton5\" type=\"string\">key="+self.line_up.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton8\" type=\"string\">key="+self.line_left.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton6\" type=\"string\">key="+self.line_right.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton7\" type=\"string\">key="+self.line_down.text()+"</merge>\n\n        <merge key=\"input.x11_options.MapButton10\" type=\"string\">key="+self.line_r2.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton9\" type=\"string\">key="+self.line_l2.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton12\" type=\"string\">key="+self.line_r1.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton11\" type=\"string\">key="+self.line_l1.text()+"</merge>\n\n        <merge key=\"input.x11_options.MapButton15\" type=\"string\">key="+self.line_cross.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton14\" type=\"string\">key="+self.line_circle.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton16\" type=\"string\">key="+self.line_square.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton13\" type=\"string\">key="+self.line_triangle.text()+"</merge>\n\n      </match>\n    </match>\n  </device>\n</deviceinfo>\n"

	self.FDI_SelectedFileLocation = QtGui.QFileDialog.getSaveFileName(self)
	if self.FDI_SelectedFileLocation.isEmpty():
            pass
	else:
	  #newFile = QtCore.QFile(self.FDI_SelectedFileLocation)
	  #if not newFile.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
	    #QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error!"), self.tr("Cannot write to file.\nPlease check if the location you selected is not read-only or if you enough space left on disk."))
	  #else:
	    self.sixadFileW = open((str(self.FDI_SelectedFileLocation)+".fdi"), "w")
	    self.sixadFileW.write(self.FDI_Content)
	    self.sixadFileW.close()
	    #newFile.writeData(self.FDI_Content)
	    #newFile.close()
	    os.system('sed -e "s/key=mouse_/button=/" -i /'+str(self.FDI_SelectedFileLocation+''))
	    os.system('sed -e "s/key=Mouse_/button=/" -i /'+str(self.FDI_SelectedFileLocation+''))
	    os.system('sed -e "s/key=MOUSE_/button=/" -i /'+str(self.FDI_SelectedFileLocation+''))
	    os.system('sed -e "s/key=none/none/" -i /'+str(self.FDI_SelectedFileLocation+''))
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Done!"), self.tr("It's done!\nA new profile has been saved.\n \nTo add the new profile, use the \"Add Profile\" button.\nFeel free to quit now"))
#	  os.system("chmod "+"777 "+str(self.FDI_SelectedFileLocation))


#----------------------
# Configure Sixaxis dialog
#----------------------------------------
class QtSixA_ConfSixaxis_Window(QtGui.QDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)

#Set icon and uic file
        self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
        uic.loadUi("/usr/share/qtsixa/gui/qtsixa_sixaxis.ui", self)

	#Read sixad configuration file
	sixad_file = commands.getoutput(". /etc/default/sixad; echo $Enable_leds $LED_js_n $LED_n $LED_plus $LED_anim $Enable_buttons $Enable_sbuttons $Enable_axis $Enable_accel $Enable_accon $Enable_speed $Enable_pos $Enable_rumble $Legacy $Debug").split()
	sixad_config_leds = sixad_file[0]
	sixad_config_led_js_n = sixad_file[1]
	sixad_config_led_n = sixad_file[2]
	sixad_config_led_plus = sixad_file[3]
	sixad_config_led_anim = sixad_file[4]
	sixad_config_buttons = sixad_file[5]
	sixad_config_sbuttons = sixad_file[6]
	sixad_config_axis = sixad_file[7]
	sixad_config_accel = sixad_file[8]
	sixad_config_accon = sixad_file[9]
	sixad_config_speed = sixad_file[10]
	sixad_config_pos = sixad_file[11]
	sixad_config_rumble = sixad_file[12]
	sixad_config_legacy = sixad_file[13]
	sixad_config_debug = sixad_file[14]

	self.i_saw_the_warning = 0
	self.applied2profile = 0
	self.applied2override = 0
	self.applied2sixad = 0
	self.applied2boot = 0
	self.applied2lr3 = 0
	self.Clicked = 0

	self.hidd_number_1 = ""
	self.hidd_number_2 = ""
	self.hidd_number_3 = ""
	self.hidd_number_4 = ""
	self.hidd_number_5 = ""
	self.hidd_number_6 = ""
	self.hidd_number_7 = ""
	self.hidd_number_8 = ""
	self.nOfDevices = 0

	self.Check4BluetoothDevices = commands.getoutput("hcitool con")
	self.nOfDevices_str = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | grep ACL -n | tail -n 1 | awk '{printf$1}' | awk 'sub(\":\",\"\")'")
	if self.nOfDevices_str != "": self.nOfDevices = int(self.nOfDevices_str) - 1
	if self.nOfDevices > 0: self.hidd_number_1 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 2 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 1: self.hidd_number_2 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 3 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 2: self.hidd_number_3 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 4 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 3: self.hidd_number_4 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 5 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 4: self.hidd_number_5 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 6 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 5: self.hidd_number_6 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 7 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 6: self.hidd_number_7 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 8 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 7: self.hidd_number_8 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 9 | tail -n 1 | awk '{printf$3}'")

	if (self.hidd_number_1 == ""):
	    self.applied2override_forced = 1
	    self.group_specific.setEnabled(0)
	    self.combo_specific_device.addItem("No Sixaxis found")
	else:
	    self.applied2override_forced = 0
	    self.group_specific.setEnabled(1)
	    if (self.hidd_number_1 != "" and commands.getoutput("hcitool name "+self.hidd_number_1+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_1)
	    if (self.hidd_number_2 != "" and commands.getoutput("hcitool name "+self.hidd_number_2+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_2)
	    if (self.hidd_number_3 != "" and commands.getoutput("hcitool name "+self.hidd_number_3+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_3)
	    if (self.hidd_number_4 != "" and commands.getoutput("hcitool name "+self.hidd_number_4+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_4)
	    if (self.hidd_number_5 != "" and commands.getoutput("hcitool name "+self.hidd_number_5+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_5)
	    if (self.hidd_number_6 != "" and commands.getoutput("hcitool name "+self.hidd_number_6+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_6)
	    if (self.hidd_number_7 != "" and commands.getoutput("hcitool name "+self.hidd_number_7+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_7)
	    if (self.hidd_number_8 != "" and commands.getoutput("hcitool name "+self.hidd_number_8+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_8)

	self.overridesN = commands.getoutput("ls /etc/hal/fdi/policy/ | grep x11-qtsixa | awk 'sub(\"x11-qtsixa_\",\"\")' | awk 'sub(\".fdi\",\"\")' | awk 'sub(\"_\",\" -> \")'")
	if (self.overridesN == ""): self.group_specific.setChecked(0)
	else: self.group_specific.setChecked(1)

	if (int(sixad_config_leds) == 0):
	    self.groupLED.setChecked(0)
	    #self.checkLED_plus.setChecked(0)
	    #self.checkLED_anim.setChecked(0)
	else:
	    self.groupLED.setChecked(1)
	    self.spinLED.setValue(int(sixad_config_led_n))
	    if (int(sixad_config_led_plus)): self.checkLED_plus.setChecked(1)
	    if (int(sixad_config_led_anim)): self.checkLED_anim.setChecked(1)
	if (int(sixad_config_buttons)): self.checkButtons.setChecked(1)
	if (int(sixad_config_sbuttons)): self.checkSButtons.setChecked(1)
	if (int(sixad_config_axis)): self.checkAxis.setChecked(1)
	if (int(sixad_config_accel)): self.checkAccel.setChecked(1)
	if (int(sixad_config_accon)): self.checkAccon.setChecked(1)
	if (int(sixad_config_speed)): self.checkSpeed.setChecked(1)
	if (int(sixad_config_pos)): self.checkPos.setChecked(1)
	if (int(sixad_config_rumble)): self.checkRumble.setChecked(1)
	if (int(sixad_config_led_js_n)):
	    self.optLEDn.setChecked(1)
	    self.frameLED.setEnabled(0)
	else:
	    self.optLEDm.setChecked(1)
	if (int(sixad_config_legacy)):
	    self.checkLegacy.setChecked(1)
	    self.groupInput.setEnabled(0)
	    self.optLEDn.setEnabled(0)
	    self.optLEDm.setChecked(1)
	if (int(sixad_config_debug)): self.checkDebug.setChecked(1)

	if commands.getoutput("if [ -f /etc/rc2.d/S90sixad ]; then echo -n 'Present'; fi") == "Present": self.checkBoot.setChecked(1)

	i = 0
	while i < len(listOfSixaxisProfiles):
	    if i % 2 == 0:
	      self.inputComboBox.addItem(listOfSixaxisProfiles[i+1])
	      self.combo_specific_profiles.addItem(listOfSixaxisProfiles[i+1])
	    i += 1

	self.current_profile_first = commands.getoutput("ls /etc/hal/fdi/policy | grep sixa | awk 'sub(\"sixa_\",\"\")' | awk 'sub(\".fdi\",\"\")'")

	if (self.current_profile_first == ""): self.current_profile = "none"
	else: self.current_profile = self.current_profile_first.split()[0]

	if (self.current_profile == "none"):
	  self.fdiProfile = "none"
	  self.inputBox.setChecked(0)
	  self.sPic.setPixmap(QtGui.QPixmap('/usr/share/qtsixa/pics/sixa_none.png'))
	else:
	  if (self.current_profile in listOfSixaxisProfiles):
	    x = listOfSixaxisProfiles.index(self.current_profile)
	    self.inputComboBox.setCurrentIndex(((x/2)+1))
	    self.fdiProfile = listOfSixaxisProfiles[x]
	    self.sPic.setPixmap(QtGui.QPixmap('/usr/share/qtsixa/pics/sixa_'+self.fdiProfile+'.png'))
	  else:
	    self.fdiProfile = "none"
	    self.inputComboBox.setCurrentIndex(0)

	if ("MapButton2" in commands.getoutput("cat /usr/share/qtsixa/sixaxis-profiles/sixa_2h4u.fdi | grep disable-mouse")):
	    self.checkLR3.setChecked(1)

	#if QtCore.PYQT_VERSION < 0x040500: self.b_new.setEnabled(0)

	self.connect(self.b_overrides, QtCore.SIGNAL("clicked()"), self.func_ShowOverrides)
	self.connect(self.b_apply, QtCore.SIGNAL('clicked()'), self.func_Apply)
	self.connect(self.b_ok, QtCore.SIGNAL('clicked()'), self.func_OK)
	self.connect(self.optLEDn, QtCore.SIGNAL("clicked()"), self.func_LED_js)
	self.connect(self.optLEDm, QtCore.SIGNAL("clicked()"), self.func_LED_man)
	self.connect(self.optLEDn, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.optLEDm, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkLED_plus, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkLED_anim, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkButtons, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkSButtons, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkAxis, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkAccel, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkAccon, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkSpeed, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkPos, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkRumble, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkLegacy, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkLegacy, QtCore.SIGNAL("clicked()"), partial(self.func_Legacy_Check))
	self.connect(self.spinLED, QtCore.SIGNAL("valueChanged(int)"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.groupLED, QtCore.SIGNAL("clicked()"), self.func_GroupLED)
	self.connect(self.checkDebug, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
	self.connect(self.checkLR3, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "lr3"))
	self.connect(self.b_new, QtCore.SIGNAL('clicked()'), self.func_NewProfile)
	self.connect(self.b_add, QtCore.SIGNAL('clicked()'), self.func_AddProfile)
	self.connect(self.inputComboBox, QtCore.SIGNAL('currentIndexChanged(QString)'), self.func_UpdateProfile)
	self.connect(self.inputComboBox, QtCore.SIGNAL('currentIndexChanged(QString)'), partial(self.func_Apply_Enable, "profile"))
	self.connect(self.inputBox, QtCore.SIGNAL('clicked()'), self.func_ClickBoxOnProfile)
	self.connect(self.inputBox, QtCore.SIGNAL('clicked()'), partial(self.func_Apply_Enable, "profile"))
	self.connect(self.checkBoot, QtCore.SIGNAL('clicked()'), partial(self.func_Apply_Enable, "boot"))
	self.connect(self.group_specific, QtCore.SIGNAL('clicked()'), partial(self.func_Apply_Enable, "override"))
	self.connect(self.combo_specific_device, QtCore.SIGNAL('currentIndexChanged(QString)'), partial(self.func_Apply_Enable, "override"))
	self.connect(self.combo_specific_profiles, QtCore.SIGNAL('currentIndexChanged(QString)'), partial(self.func_Apply_Enable, "override"))

    def func_ShowOverrides(self):
	self.overrides = commands.getoutput("ls /etc/hal/fdi/policy/ | grep x11-qtsixa | awk 'sub(\"x11-qtsixa_\",\"\")' | awk 'sub(\".fdi\",\"\")' | awk 'sub(\"_\",\" -> \")'")
	if (self.overrides == ""):
	  QtGui.QMessageBox.information(self, self.tr("QtSixA - Show"), self.tr("No current overrides are set."))
	else:
	  QtGui.QMessageBox.information(self, self.tr("QtSixA - Show"), self.tr("Current overrides:\n \n"+self.overrides+"\n"))

    def func_Apply_Enable(self, what2apply):
	self.b_apply.setEnabled(1)
	self.b_cancel.setEnabled(1)
	if (what2apply == "sixad"): self.applied2sixad = 1
	elif (what2apply == "profile"): self.applied2profile = 1
	elif (what2apply == "override"): self.applied2override = 1
	elif (what2apply == "boot"): self.applied2boot = 1
	elif (what2apply == "lr3"): self.applied2lr3 = 1

    def func_Legacy_Check(self):
	if self.checkLegacy.isChecked():
	    self.groupInput.setEnabled(0)
	    self.optLEDn.setEnabled(0)
	    self.optLEDm.setChecked(1)
	    self.frameLED.setEnabled(1)
	else:
	    self.groupInput.setEnabled(1)
	    self.optLEDn.setEnabled(1)

    def func_LED_man(self):
	self.frameLED.setEnabled(1)

    def func_LED_js(self):
	self.frameLED.setEnabled(0)

    def func_OK(self):
	self.func_Apply()
	self.close()

    def func_Apply(self):
	if self.applied2override == 1 and self.applied2override_forced == 0: #overrides
	    if self.group_specific.isChecked():
		self.specificDevice = self.combo_specific_device.currentText()
		self.specificProfile_str = self.combo_specific_profiles.currentText()
		if (self.specificProfile_str == "None"):
		    self.specificProfile = "none"
		else:
		    if (self.specificProfile_str in listOfSixaxisProfiles):
			xxx = listOfSixaxisProfiles.index(self.specificProfile_str)
			self.specificProfile = listOfSixaxisProfiles[xxx-1]
		    else:
			QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error"), self.tr("The selected profile is not on the list of profiles!\nQtSixA will quit now!"))
			self.close()
			exit(-1)
		if look4Root():
		    os.system(ROOT+" cp /usr/share/qtsixa/sixaxis-profiles/sixa_"+str(self.specificProfile)+".fdi /etc/hal/fdi/policy/x11-qtsixa_"+str(self.specificDevice)+"_"+str(self.specificProfile)+".fdi")
		    os.system(ROOT+' sed -e "s/ contains/ string_outof/" -i /etc/hal/fdi/policy/x11-qtsixa_'+str(self.specificDevice)+'_'+str(self.specificProfile)+'.fdi')
		    os.system(ROOT+' sed -e "s/PLAYSTATION(R)3 Controller/PLAYSTATION(R)3 Controller ('+str(self.specificDevice)+')/" -i /etc/hal/fdi/policy/x11-qtsixa_'+str(self.specificDevice)+'_'+str(self.specificProfile)+'.fdi')
		    QtGui.QMessageBox.information(self, self.tr("QtSixA - Done"), self.tr("The override has been set:\n \nDevice: "+self.specificDevice+"\nProfile: "+self.specificProfile_str+"\n"))
	    else:
		if look4Root(): os.system(ROOT+" rm -rf /etc/hal/fdi/policy/x11-qtsixa_*.fdi")

	if self.applied2profile == 1: #Sixaxis profile stuff
	    if look4Root():
		os.system(ROOT+" rm -rf /etc/hal/fdi/policy/sixa*.fdi")
		os.system(ROOT+" cp /usr/share/qtsixa/sixaxis-profiles/sixa_"+self.fdiProfile+".fdi "+"/etc/hal/fdi/policy/")
		QtGui.QMessageBox.information(self, self.tr("QtSixA - Profile"), self.tr("Input profile has now been set to "+self.inputComboBox.currentText()+".\n \nIf you want to use it now, please disconnect your Sixaxis\nand connect them again."))


	if self.applied2boot == 1: #Enable/Disable at boot
	    if look4Root():
		if self.checkBoot.isChecked(): os.system(ROOT+" sixad "+"--boot-yes")
		else: os.system(ROOT+" sixad "+"--boot-no")


	if self.applied2lr3 == 1: #Enable/Disable LR3
	    if look4Root():
		if self.checkLR3.isChecked():
		    os.system(ROOT+" sed \"s/MapButton2\\\" type=\\\"string\\\">none</MapButton2\\\" type=\\\"string\\\">disable-mouse</\" -i /etc/hal/fdi/policy/sixa_*.fdi")
		    os.system(ROOT+" sed \"s/MapButton3\\\" type=\\\"string\\\">none</MapButton3\\\" type=\\\"string\\\">disable-keys</\" -i /etc/hal/fdi/policy/sixa_*.fdi")
		    os.system(ROOT+" sed \"s/MapButton2\\\" type=\\\"string\\\">none</MapButton2\\\" type=\\\"string\\\">disable-mouse</\" -i /usr/share/qtsixa/sixaxis-profiles/sixa_*.fdi")
		    os.system(ROOT+" sed \"s/MapButton3\\\" type=\\\"string\\\">none</MapButton3\\\" type=\\\"string\\\">disable-keys</\" -i /usr/share/qtsixa/sixaxis-profiles/sixa_*.fdi")
		else:
		    os.system(ROOT+" sed \"s/MapButton2\\\" type=\\\"string\\\">disable-mouse</MapButton2\\\" type=\\\"string\\\">none</\" -i /etc/hal/fdi/policy/sixa_*.fdi")
		    os.system(ROOT+" sed \"s/MapButton3\\\" type=\\\"string\\\">disable-keys<</MapButton3\\\" type=\\\"string\\\">none</\" -i /etc/hal/fdi/policy/sixa_*.fdi")
		    os.system(ROOT+" sed \"s/MapButton2\\\" type=\\\"string\\\">disable-mouse</MapButton2\\\" type=\\\"string\\\">none</\" -i /usr/share/qtsixa/sixaxis-profiles/sixa_*.fdi")
		    os.system(ROOT+" sed \"s/MapButton3\\\" type=\\\"string\\\">disable-keys<</MapButton3\\\" type=\\\"string\\\">none</\" -i /usr/share/qtsixa/sixaxis-profiles/sixa_*.fdi")

	if self.applied2sixad == 1: #sixad settings
#starts here----------------------------
	    if self.groupLED.isChecked(): self.txtLEDs = "1"
	    else: self.txtLEDs = "0"
	    if self.optLEDn.isChecked(): self.txtLED_js = "1"
	    else: self.txtLED_js = "0"
	    self.txtLED_n = str(self.spinLED.text())
	    if self.checkLED_plus.isChecked(): self.txtLED_plus = "1"
	    else: self.txtLED_plus = "0"
	    if self.checkLED_anim.isChecked(): self.txtLED_anim = "1"
	    else: self.txtLED_anim = "0"
	    if self.checkButtons.isChecked(): self.txtEnable_buttons = "1"
	    else: self.txtEnable_buttons = "0"
	    if self.checkSButtons.isChecked(): self.txtEnable_sbuttons = "1"
	    else: self.txtEnable_sbuttons = "0"
	    if self.checkAxis.isChecked(): self.txtEnable_axis = "1"
	    else: self.txtEnable_axis = "0"
	    if self.checkAccel.isChecked(): self.txtEnable_accel = "1"
	    else: self.txtEnable_accel = "0"
	    if self.checkAccon.isChecked(): self.txtEnable_accon = "1"
	    else: self.txtEnable_accon = "0"
	    if self.checkSpeed.isChecked(): self.txtEnable_speed = "1"
	    else: self.txtEnable_speed = "0"
	    if self.checkPos.isChecked(): self.txtEnable_pos = "1"
	    else: self.txtEnable_pos = "0"
	    if self.checkRumble.isChecked(): self.txtEnable_rumble = "1"
	    else: self.txtEnable_rumble = "0"
	    if self.checkLegacy.isChecked(): self.txtEnable_legacy = "1"
	    else: self.txtEnable_legacy = "0"
	    if self.checkDebug.isChecked(): self.txtEnable_debug = "1"
	    else: self.txtEnable_debug = "0"

	    self.sixadFile = (""
	    "# sixad configuration file\n"
	    "#\n"
	    "# Please use \"1\" or \"0\" instead of \"yes/true\" or \"no/false\"\n"
	    "#  (this values are sent to C code...)\n"
	    "\n"
	    "\n"
	    "# Enable LEDs?\n"
	    "Enable_leds="+self.txtLED_n+"\n"
	    "\n"
	    "# Use js # for LED #? (overrides \"LED_n\" and \"LED_plus\"; doesn't work with \"Legacy\" )\n"
	    "LED_js_n="+self.txtLED_js+"\n"
	    "\n"
	    "# Start with LED #\n"
	    "LED_n="+self.txtLED_n+"\n"
	    "\n"
	    "# LED # increase after new connection?\n"
	    "LED_plus="+self.txtLED_plus+"\n"
	    "\n"
	    "# Enable LEDs/Rumble animation?\n"
	    "LED_anim="+self.txtLED_anim+"\n"
	    "\n"
	    "# Enable buttons?\n"
	    "Enable_buttons="+self.txtEnable_buttons+"\n"
	    "\n"
	    "# Enable sensible buttons? (as axis)\n"
	    "Enable_sbuttons="+self.txtEnable_sbuttons+"\n"
	    "\n"
	    "# Enable axis? (Left & Right)\n"
	    "Enable_axis="+self.txtEnable_axis+"\n"
	    "\n"
	    "# Enable accelerometers?\n"
	    "Enable_accel="+self.txtEnable_accel+"\n"
	    "\n"
	    "# Enable acceleration?\n"
	    "Enable_accon="+self.txtEnable_accon+"\n"
	    "\n"
	    "# Enable speed?\n"
	    "Enable_speed="+self.txtEnable_speed+"\n"
	    "\n"
	    "# Enable position?\n"
	    "Enable_pos="+self.txtEnable_pos+"\n"
	    "\n"
	    "# Enable rumble? (Incomplete)\n"
	    "Enable_rumble="+self.txtEnable_rumble+"\n"
	    "\n"
	    "# Use Old/Legacy driver instead of sixad (Workaround for PowerPC[32] UInput)\n"
	    "Legacy="+self.txtEnable_legacy+"\n"
	    "\n"
	    "# Enable debug?\n"
	    "Debug="+self.txtEnable_debug+"\n"
	    "")

	    self.sixadFileW = open("/tmp/sixad", "w")
	    self.sixadFileW.write(self.sixadFile)
	    self.sixadFileW.close()
	    if look4Root(): os.system(ROOT+" cp /tmp/sixad /etc/default/sixad")

	    if self.i_saw_the_warning == 0:
	      #QtGui.QMessageBox.information(self, self.tr("QtSixA - Done"), self.tr(""
	      #"You have changed sixad settings, which will only be applied on the next connected Sixaxis"))
	      if commands.getoutput("uname -m") == "powerpc" and not self.checkLegacy.isChecked():
		  QtGui.QMessageBox.warning(self, self.tr("QtSixA - Caution!"), self.tr(""
		  "These Sixaxis settings are invalid!<br>"
		  "<b>Please enable \"Legacy Driver\" when running from PowerPC!</b>"))
	    self.i_saw_the_warning = 1
#ends here--------------------------------------------------------------------

	self.applied2profile = 0
	self.applied2override = 0
	self.applied2sixad = 0
	self.applied2boot = 0
	self.applied2lr3 = 0
	self.b_cancel.setEnabled(0)
	self.b_apply.setEnabled(0)

    def func_GroupLED(self):
	if self.groupLED.isChecked():
	    self.checkLED_plus.setEnabled(1)
	    self.checkLED_anim.setEnabled(1)
	else:
	    self.checkLED_plus.setEnabled(0)
	    self.checkLED_anim.setEnabled(0)
	self.func_Apply_Enable("sixad")

    def func_NewProfile(self):
        QtSixA_Profile_New().exec_()

    def func_AddProfile(self):
        QtSixA_Add_Window().exec_()

    def func_ClickBoxOnProfile(self):
        if (self.Clicked == "0"):
            self.Clicked = "1"
            self.fdiProfile = "none"
            self.inputComboBox.setCurrentIndex(0)
        else:
            self.Clicked = "0"
            self.inputComboBox.setCurrentIndex(0)

    def func_UpdateProfile(self):
        if (self.inputComboBox.currentText() == "None"):
            self.sPic.setPixmap(QtGui.QPixmap('/usr/share/qtsixa/pics/sixa_none.png'))
            self.fdiProfile = "none"
        else:
            if (self.inputComboBox.currentText() in listOfSixaxisProfiles):
                x = listOfSixaxisProfiles.index(self.inputComboBox.currentText())
                self.fdiProfile = listOfSixaxisProfiles[x-1]
                self.sPic.setPixmap(QtGui.QPixmap('/usr/share/qtsixa/pics/sixa_'+self.fdiProfile+'.png'))
            else:
                self.fdiProfile = "none"
                self.inputComboBox.setCurrentIndex(0)


#----------------------
# Configure QtSixA dialog
#----------------------------------------
class QtSixA_ConfQtSixA_Window(QtGui.QDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)

#Set icon and uic file
        self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
        uic.loadUi("/usr/share/qtsixa/gui/qtsixa_options.ui", self)

	#RE-Read preferences file
	config_file = commands.getoutput('cat "$HOME/.qtsixa/qtsixa.conf"').split()
	config_enable_systray = config_file[1]
	config_start_minimized = config_file[3]
	config_close_to_tray = config_file[5]
	config_display_info = config_file[7]

	self.i_saw_the_warning = 0

	#Notification stuff
	if (commands.getoutput("ls -a /tmp | grep sixa-notify") != ""):
	    self.box_notify.setChecked(1)
	    self.box_notify_start.setEnabled(1)
	    self.alreadyStartedNotify = 1
	else:
	    self.alreadyStartedNotify = 0
	if (commands.getoutput("ls $HOME/.config/autostart/ | grep sixa-notify") != ""):
	    self.box_notify.setChecked(1)
	    self.box_notify_start.setEnabled(1)
	    self.box_notify_start.setChecked(1)

	if (commands.getoutput("ls $HOME/.config/autostart/ | grep qtsixa.desktop") != ""):
	    self.box_startup.setChecked(1)

	if config_enable_systray == "1":  self.box_systray.setChecked(1)
	if config_enable_systray == "1" and config_start_minimized == "1":  self.box_min.setChecked(1)
	if config_enable_systray == "1" and config_close_to_tray == "1":  self.box_close.setChecked(1)
	if config_display_info == "1":  self.box_info.setChecked(1)

	if self.box_systray.isChecked():
	  self.box_min.setEnabled(1)
	  self.box_close.setEnabled(1)
	else:
	  self.box_min.setChecked(0)
	  self.box_min.setEnabled(0)
	  self.box_close.setEnabled(0)
	  self.box_close.setChecked(0)

	self.connect(self.b_apply, QtCore.SIGNAL('clicked()'), self.func_Apply)
	self.connect(self.b_ok, QtCore.SIGNAL('clicked()'), self.func_OK)
	self.connect(self.box_startup, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
	self.connect(self.box_min, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
	self.connect(self.box_info, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
	self.connect(self.box_close, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
	self.connect(self.box_systray, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
	self.connect(self.box_systray, QtCore.SIGNAL('clicked()'), self.func_Systray)

	self.connect(self.box_notify, QtCore.SIGNAL("clicked()"), self.func_NotifyBox)
	self.connect(self.box_notify_start, QtCore.SIGNAL("clicked()"), self.func_Apply_Enable)

    def func_Apply_Enable(self):
	  self.b_apply.setEnabled(1)
	  self.b_cancel.setEnabled(1)

    def func_NotifyBox(self):
	if self.box_notify.isChecked():
	    self.box_notify_start.setEnabled(1)
	else:
	    self.box_notify_start.setEnabled(0)
	    self.box_notify_start.setChecked(0)
	self.func_Apply_Enable()

    def func_Systray(self):
	if self.box_systray.isChecked():
	  self.box_min.setEnabled(1)
	  self.box_close.setEnabled(1)
	else:
	  self.box_min.setChecked(0)
	  self.box_min.setEnabled(0)
	  self.box_close.setEnabled(0)
	  self.box_close.setChecked(0)

    def func_OK(self):
	if self.b_apply.isEnabled(): self.func_Apply()
	self.close()

    def func_Apply(self):

	if self.box_systray.isChecked(): self.conf_systray = "1"
	else: self.conf_systray = "0"
	if self.box_min.isChecked(): self.conf_min = "1"
	else: self.conf_min = "0"
	if self.box_close.isChecked(): self.conf_close = "1"
	else: self.conf_close = "0"
	if self.box_info.isChecked(): self.conf_info = "1"
	else: self.conf_info = "0"

	self.finalFile = (""
	"enable_systray	"+self.conf_systray+"\n"
	"start_minimized	"+self.conf_min+"\n"
	"close_to_tray	"+self.conf_close+"\n"
	"display_info	"+self.conf_info+"\n"
	"")

	if self.box_notify.isChecked():
	    if (self.alreadyStartedNotify == 0):
		os.system("rm -rf "+"/tmp/.sixa-notify")
		os.system("sixa-notify &")
	    if self.box_notify_start.isChecked():
		os.system("cp /usr/share/qtsixa/sixa-notify.desktop $HOME/.config/autostart/sixa-notify.desktop")
	    else:
		os.system("rm -rf $HOME/.config/autostart/sixa-notify.desktop")
	    self.alreadyStartedNotify = 1
	else:
	    os.system("rm -rf /tmp/.sixa-notify")
	    os.system("rm -rf $HOME/.config/autostart/sixa-notify.desktop")
	    self.alreadyStartedNotify = 0

	if self.box_startup.isChecked(): os.system("cp /usr/share/applications/qtsixa.desktop $HOME/.config/autostart/qtsixa.desktop")
	else: os.system("rm -rf $HOME/.config/autostart/qtsixa.desktop")

	saveFile = QtCore.QFile(commands.getoutput("ls $HOME/.qtsixa/qtsixa.conf").split()[0])

	if not saveFile.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
	  QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error!"), self.tr("Cannot write to configuration file.\nPlease check if your media is not read-only, enough space left on disk and permissions."))
	else:
	  os.system("echo '"+str(self.finalFile)+"' > $HOME/.qtsixa/qtsixa.conf")
	  if self.i_saw_the_warning == 0:
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Done"), self.tr(""
	    "Don't forget you'll have to restart QtSixA to apply this changes."))

	self.i_saw_the_warning = 1
	self.b_cancel.setEnabled(0)
	self.b_apply.setEnabled(0)


#------------
# Sixpair setup
#-----------------------------------------
class QtSixA_Sixpair_Window(QtGui.QDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)
        
#Set icon and uic file    
	self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
	uic.loadUi("/usr/share/qtsixa/gui/qtsixa_sixpair.ui", self)

	self.connect(self.b_cancel, QtCore.SIGNAL("clicked()"), QtCore.SLOT('close()'))
	self.connect(self.b_back, QtCore.SIGNAL("clicked()"), self.func_GoToPage1)
	self.connect(self.b_next, QtCore.SIGNAL("clicked()"), self.func_GoToPage2)
	self.connect(self.b_finish, QtCore.SIGNAL("clicked()"), QtCore.SLOT('close()'))

	self.textEdit.setReadOnly(True)
	self.CurrentPage = "0"
	self.devName = "Sixaxis"

	self.func_GoToPage1()

    def func_GoToPage1(self):
	self.CurrentPage = "1"
	self.label.setText('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd"> <html><head><meta name="qrichtext" content="1" /><style type="text/css"> p, li { white-space: pre-wrap; } </style></head><body style=" font-size:9pt; font-weight:400; font-style:normal;"> <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><span style=" font-weight:600;">Getting ready for Sixpair setup...</span> </p> <p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"></p> <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">Before continue please make sure that your bluetooth</p> <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">stick/device/pen is connected to the PC and that the Sixaxis/Keypad</p> <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">is connected to the PC\'s USB</p> <p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"></p> <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><span style=" font-style:italic;">Note: only one Sixaxis/Keypad per setup is supported</span></p></body></html>')
	self.b_cancel.setEnabled(1)
	self.b_back.setEnabled(0)
	self.b_next.setEnabled(1)
	self.b_finish.setEnabled(0)
	self.textEdit.setVisible(0)
	self.groupDevice.setVisible(1)

    def func_GoToPage2(self):
	self.CurrentPage = "2"
	self.b_cancel.setEnabled(0)
	self.b_back.setEnabled(0)
	self.b_next.setEnabled(0)
	self.b_finish.setEnabled(0)
	self.textEdit.setVisible(1)
	self.groupDevice.setVisible(0)
	if self.radio_dev_sixaxis.isChecked():
	    if look4Root(): self.sixpair_report = commands.getoutput(ROOT+" /usr/sbin/sixpair")
	    else: self.sixpair_report = "Not enough rights"
	    self.devName = "Sixaxis"
	else:
	    if look4Root(): self.sixpair_report = commands.getoutput(ROOT+" /usr/sbin/sixpair-kbd")
	    else: self.sixpair_report = "Not enough rights"
	    self.devName = "Keypad"
	self.textEdit.setText(self.sixpair_report)

	if (self.sixpair_report == "Not enough rights"):
	  self.label.setText("Sixpair needs root/admin privileges to run\n \nPlease go back or cancel.")
	  self.b_cancel.setEnabled(1)
	  self.b_back.setEnabled(1)
	  self.b_next.setEnabled(0)
	  self.b_finish.setEnabled(0)
	elif ("found on USB busses" in self.sixpair_report):
	  self.label.setText("Sixpair reports that no "+self.devName+" was found.\nIt seems like you forgot something...\n \nPlease go back or cancel.\n \n \nThe sixpair report:")
	  self.b_cancel.setEnabled(1)
	  self.b_back.setEnabled(1)
	  self.b_next.setEnabled(0)
	  self.b_finish.setEnabled(0)
	else:
	  self.label.setText('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd"> <html><head><meta name="qrichtext" content="1" /><style type="text/css"> p, li { white-space: pre-wrap; } </style></head><body style=" font-size:9pt; font-weight:400; font-style:normal;"> <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">You\'re bluetooth stick/pen/device should now be</p> <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">paired with the '+self.devName+'.</p> <p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"></p> <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">Below you can see the sixpair report:</p> </body></html>')
	  self.b_cancel.setEnabled(0)
	  self.b_back.setEnabled(1)
	  self.b_next.setEnabled(0)
	  self.b_finish.setEnabled(1)


#--------------
# The Main Window!
#-------------------------------------------
class Main_QtSixA_Window(QtGui.QMainWindow):
    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
    
#Set icon and uic file
	self.setWindowIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa.png'))
	uic.loadUi("/usr/share/qtsixa/gui/qtsixa_main.ui", self)

	self.autoListRefresh = QtCore.QTimer()
	self.autoLQRefresh = QtCore.QTimer()

	self.connect(self.b_disconnect, QtCore.SIGNAL('clicked()'), self.func_Disconnect)
	self.connect(self.b_battery, QtCore.SIGNAL('clicked()'), self.func_Battery)
	self.connect(self.actionDiscAllSixaxis, QtCore.SIGNAL('triggered()'), self.func_DiscAllSixaxis)
	self.connect(self.actionDiscEver, QtCore.SIGNAL('triggered()'), self.func_DiscEverything)
	self.connect(self.actionDiscOne, QtCore.SIGNAL("triggered()"), partial(self.func_DiscSelected, 1))
	self.connect(self.actionDiscTwo, QtCore.SIGNAL("triggered()"), partial(self.func_DiscSelected, 2))
	self.connect(self.actionDiscThree, QtCore.SIGNAL("triggered()"), partial(self.func_DiscSelected, 3))
	self.connect(self.actionDiscFour, QtCore.SIGNAL("triggered()"), partial(self.func_DiscSelected, 4))
	self.connect(self.actionDiscFive, QtCore.SIGNAL("triggered()"), partial(self.func_DiscSelected, 5))
	self.connect(self.actionDiscSix, QtCore.SIGNAL("triggered()"), partial(self.func_DiscSelected, 6))
	self.connect(self.actionDiscSeven, QtCore.SIGNAL("triggered()"), partial(self.func_DiscSelected, 7))
	self.connect(self.actionDiscEight, QtCore.SIGNAL("triggered()"), partial(self.func_DiscSelected, 8))
	self.connect(self.actionBT_Start, QtCore.SIGNAL('triggered()'), self.func_BT_Start)
	self.connect(self.actionBT_Stop, QtCore.SIGNAL('triggered()'), self.func_BT_Stop)
	self.connect(self.actionSixpair, QtCore.SIGNAL('triggered()'), self.func_Sixpair)
	self.connect(self.actionForce, QtCore.SIGNAL('triggered()'), self.func_Force)
	self.connect(self.actionStop, QtCore.SIGNAL('triggered()'), self.func_Stop)
	self.connect(self.actionExit, QtCore.SIGNAL('triggered()'), QtCore.SLOT('close()'))
	self.connect(self.actionConfSixaxis, QtCore.SIGNAL('triggered()'), self.func_ConfSixaxis)
	self.connect(self.actionConfQtSixA, QtCore.SIGNAL('triggered()'), self.func_ConfQtSixA)
	self.connect(self.actionClearBT, QtCore.SIGNAL('triggered()'), self.func_ClearBT)
	self.connect(self.actionRestoreDef, QtCore.SIGNAL('triggered()'), self.func_RestoreDef)
	self.connect(self.actionSixaxisResPro, QtCore.SIGNAL('triggered()'), self.func_RestoreProfiles)
	self.connect(self.actionManual, QtCore.SIGNAL('triggered()'), self.func_Manual)
	self.connect(self.actionSourceForge, QtCore.SIGNAL('triggered()'), self.func_SourceForge)
	self.connect(self.actionUbuntu, QtCore.SIGNAL('triggered()'), self.func_UbuntuForums)
	self.connect(self.actionDonate, QtCore.SIGNAL('triggered()'), self.func_Donate)
	self.connect(self.actionReport_Bug, QtCore.SIGNAL('triggered()'), self.func_Report_Bug)
	self.connect(self.actionIdea, QtCore.SIGNAL('triggered()'), self.func_Idea)
	self.connect(self.actionAskSome, QtCore.SIGNAL('triggered()'), self.func_Questions)
	self.connect(self.actionReference, QtCore.SIGNAL('triggered()'), self.func_Reference)
	self.connect(self.actionList_of_Features, QtCore.SIGNAL('triggered()'), self.func_Features)
	self.connect(self.actionAbout_QtSixA, QtCore.SIGNAL('triggered()'), self.func_About_QtSixA)
	self.connect(self.actionAbout_Qt, QtCore.SIGNAL('triggered()'), QtGui.qApp, QtCore.SLOT("aboutQt()"))

	self.connect(self.b_game_help, QtCore.SIGNAL("clicked()"), self.func_HelpGame)
	self.connect(self.b_game_apply, QtCore.SIGNAL("clicked()"), self.func_ApplyGame)
	self.connect(self.b_apply_signal, QtCore.SIGNAL("clicked()"), self.func_Apply_signal)
	self.connect(self.b_apply_hidraw, QtCore.SIGNAL("clicked()"), self.func_Apply_hidraw)
	self.connect(self.b_refresh_signal, QtCore.SIGNAL("clicked()"), self.func_Refresh_signal)
	self.connect(self.b_refresh_hidraw, QtCore.SIGNAL("clicked()"), self.func_Refresh_hidraw)
	self.connect(self.b_reset_signal, QtCore.SIGNAL("clicked()"), self.func_Reset_signal)
	self.connect(self.b_tips_signal, QtCore.SIGNAL("clicked()"), self.func_Tips_signal)
	self.connect(self.b_stop_sixadraw, QtCore.SIGNAL("clicked()"), self.func_Kill_sixadraw)

	self.connect(self.autoListRefresh, QtCore.SIGNAL('timeout()'), self.func_UpdateListOfDevices)
	self.connect(self.autoLQRefresh, QtCore.SIGNAL('timeout()'), self.func_UpdateDeviceLQ)
	self.connect(self.listOfDevices, QtCore.SIGNAL('currentRowChanged(int)'), self.func_UpdateDeviceStats)
	self.connect(self.listOfGames, QtCore.SIGNAL('currentIndexChanged(int)'), self.func_UpdateGames)
	self.connect(self.listOfGames, QtCore.SIGNAL('currentIndexChanged(int)'), self.func_Game_bOff)

	self.connect(self.radio_etracer_axis, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_etracer_accel, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_etracer_full, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_stk_digital, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_stk_axis, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_stk_accel, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_stk_full, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_neverball_axis, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_neverball_accel, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_game_epsxe_1, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_game_epsxe_axis, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)
	self.connect(self.radio_game_epsxe_drv, QtCore.SIGNAL("clicked()"), self.func_Game_bOn)

	self.hidd_number_1 = ""
	self.hidd_number_2 = ""
	self.hidd_number_3 = ""
	self.hidd_number_4 = ""
	self.hidd_number_5 = ""
	self.hidd_number_6 = ""
	self.hidd_number_7 = ""
	self.hidd_number_8 = ""
	self.usb_number_1 = ""
	self.usb_number_2 = ""
	self.usb_number_3 = ""
	self.usb_number_4 = ""
	self.trayTooltip = "If you see this message, then something is not working as it should"
	self.SixaxisProfile = ""
	self.trayIsActive = 0
	#self.wEP.setVisible(1)
	self.wET.setVisible(0)
	self.wNE.setVisible(0)
	self.wSTK.setVisible(0)
	self.b_game_apply.setEnabled(0)

	self.func_UpdateListOfDevices()
	self.func_UpdateDeviceStats()
	self.autoListRefresh.start(2000)
	self.func_What()

	if config_display_info == "1":  QtGui.QMessageBox.information(self, self.tr("QtSixA - Updated"), self.tr(""
	"<b>QtSixA has been updated to 1.0.2</b>.<br>"
	"This is the final stable version - Enjoy!<p>"
	"If you found any bug, please report it using:<br>"
	"\"Help\" -> \"Web Links\" -> \"Report bug\"<p>"
	"<i>(To disable this pop-up, go to \"Settings\" -> \"Configure QtSixA\")</i>"
	""))

	if config_enable_systray == "1":
	    self.createTrayIcon()
	    self.trayIsActive = 1
	else:
	    self.trayIsActive = 0


    def func_What(self):
	self.SixaxisProfileCheck = commands.getoutput("ls /etc/hal/fdi/policy | grep sixa | awk 'sub(\"sixa_\",\"\")' | awk 'sub(\".fdi\",\"\")'")
	if (self.SixaxisProfileCheck == ""):
	    self.label_SixaxisProfile.setText("You're not using a Sixaxis profile")
	else:
	    self.SixaxisProfile = self.SixaxisProfileCheck.split()[0]
	    if (self.SixaxisProfile in listOfSixaxisProfiles):
		x = listOfSixaxisProfiles.index(self.SixaxisProfile)
		self.SixaxisProfile = listOfSixaxisProfiles[x+1]
		self.label_SixaxisProfile.setText("Current Sixaxis profile is "+self.SixaxisProfile)
	    else: self.label_SixaxisProfile.setText("You're not using a Sixaxis profile")

    def func_DBusDisconnect(self, mode, name):
	try:
	    bluez_bus = bus.get_object('org.bluez', '/')
	    bluez_id = bluez_bus.DefaultAdapter(dbus_interface='org.bluez.Manager')
	    adapter_bus = bus.get_object('org.bluez', bluez_id)
	    listDev = adapter_bus.ListDevices(dbus_interface='org.bluez.Adapter')
	    #self.func_UpdateDeviceStats()
	except:
	    listDev = ""
	    print "Could not disconnect some devices, please wait a few seconds and try again"
	    QtGui.QMessageBox.warning(self, self.tr("QtSixA - Disconnect"), self.tr(""
	    "Could not disconect some devices.<br>"
	    "<i>(Maybe bluetooth is off?)</i>"))

	j = 0
	while j < len(listDev):
	    device_bus = bus.get_object('org.bluez', listDev[j])
	    idev = dbus.Interface(device_bus, dbus_interface='org.bluez.Device')
	    if mode == "single":
		if name in listDev[j]: idev.Disconnect()
	    elif mode == "sixaxis":
		if "PLAYSTATION(R)3 Controller" in str(idev.GetProperties()): idev.Disconnect()
	    elif mode == "all":
		idev.Disconnect()
	    j += 1


    def func_Disconnect(self):
        self.DeviceToDisconnect = self.listOfDevices.currentRow()
        if (self.DeviceToDisconnect == 1): self.selectedDevice = self.hidd_number_1
        elif (self.DeviceToDisconnect == 2): self.selectedDevice = self.hidd_number_2
        elif (self.DeviceToDisconnect == 3): self.selectedDevice = self.hidd_number_3
        elif (self.DeviceToDisconnect == 4): self.selectedDevice = self.hidd_number_4
        elif (self.DeviceToDisconnect == 5): self.selectedDevice = self.hidd_number_5
        elif (self.DeviceToDisconnect == 6): self.selectedDevice = self.hidd_number_6
        elif (self.DeviceToDisconnect == 7): self.selectedDevice = self.hidd_number_7
        elif (self.DeviceToDisconnect == 8): self.selectedDevice = self.hidd_number_8
	else:
	    print "Device not connected; Cannot disconnect"
	    return
	self.selectedDeviceParsed = commands.getoutput("echo "+self.selectedDevice+" | awk 'sub(\":\",\"_\")' | awk 'sub(\":\",\"_\")' | awk 'sub(\":\",\"_\")' | awk 'sub(\":\",\"_\")' | awk 'sub(\":\",\"_\")' ")
	self.func_DBusDisconnect("single", self.selectedDeviceParsed)
	self.listOfDevices.setCurrentRow(-1)

    def func_Battery(self):
	self.label_bat.setEnabled(1)
	self.barBattery.setEnabled(1)
	self.barBattery.setTextVisible(1)
	self.barBattery.setMaximum(5)
        self.DeviceToCheckBattery = self.listOfDevices.currentRow()
        if (self.DeviceToCheckBattery == 1): self.DeviceToCheck = self.hidd_number_1
	elif (self.DeviceToCheckBattery == 2): self.DeviceToCheck = self.hidd_number_2
	elif (self.DeviceToCheckBattery == 3): self.DeviceToCheck = self.hidd_number_3
	elif (self.DeviceToCheckBattery == 4): self.DeviceToCheck = self.hidd_number_4
	elif (self.DeviceToCheckBattery == 5): self.DeviceToCheck = self.hidd_number_5
	elif (self.DeviceToCheckBattery == 6): self.DeviceToCheck = self.hidd_number_6
	elif (self.DeviceToCheckBattery == 7): self.DeviceToCheck = self.hidd_number_7
	elif (self.DeviceToCheckBattery == 8): self.DeviceToCheck = self.hidd_number_8
	else: print "Device not connected; Cannot check battery"
	if look4Root(): self.SixaxisBat = commands.getoutput(ROOT+" hcidump "+"-R "+"-O '"+self.DeviceToCheck+"' "+"| "+"head "+"-n "+"5 "+"| "+"tail "+"-n "+"1 "+"| "+"awk "+"'{printf$1}' "+"& "+"sleep "+"1 "+"&& "+ROOT+" killall "+"hcidump "+"> "+"/dev/null")
	else: self.SixaxisBat = ""
	if not "o" in self.SixaxisBat and self.SixaxisBat != "":
	    if self.SixaxisBat == "EE": self.barBattery.setMaximum(0)
	    elif self.SixaxisBat =="HCI": print "Device not connected; Cannot check battery (2)" #(2) - to know what is the exact error
	    else: self.barBattery.setValue(int(self.SixaxisBat))

    def func_DiscAllSixaxis(self):
	self.func_DBusDisconnect("sixaxis", "NULL")
	self.listOfDevices.setCurrentRow(-1)
        #QtGui.QMessageBox.information(self, self.tr("QtSixA - Disconnect"), self.tr("All Sixaxis should now be disconected."))

    def func_DiscEverything(self):
	self.func_DBusDisconnect("all", "NULL")
	self.listOfDevices.setCurrentRow(-1)
        #if listDev != "": QtGui.QMessageBox.information(self, self.tr("QtSixA - Disconnect"), self.tr("All bluetooth devices should now be disconected."))
	#else: QtGui.QMessageBox.warning(self, self.tr("QtSixA - Disconnect"), self.tr("Could not disconect all devices. Please try again later"))

    def func_DiscSelected(self, number):
	if number == 1: self.selectedDevice = self.hidd_number_1
	if number == 2: self.selectedDevice = self.hidd_number_2
	if number == 3: self.selectedDevice = self.hidd_number_3
	if number == 4: self.selectedDevice = self.hidd_number_4
	if number == 5: self.selectedDevice = self.hidd_number_5
	if number == 6: self.selectedDevice = self.hidd_number_6
	if number == 7: self.selectedDevice = self.hidd_number_7
	if number == 8: self.selectedDevice = self.hidd_number_8
	self.selectedDeviceParsed = commands.getoutput("echo "+self.selectedDevice+" | awk 'sub(\":\",\"_\")' | awk 'sub(\":\",\"_\")' | awk 'sub(\":\",\"_\")' | awk 'sub(\":\",\"_\")' | awk 'sub(\":\",\"_\")' ")
	self.func_DBusDisconnect("single", self.selectedDeviceParsed)
	self.listOfDevices.setCurrentRow(-1)

    def func_BT_Start(self):
	if look4Root():
	    os.system("if [ -f /lib/udev/rules.d/97-bluetooth.rules ]; then "+ROOT+" bluetoothd --udev; else "+ROOT+" /etc/init.d/bluetooth start; fi")
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Started"), self.tr("Bluetooth should now be started (restored)"))

    def func_BT_Stop(self):
	if look4Root():
	    os.system(ROOT+" pkill -KILL bluetoothd")
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Stopped"), self.tr("Bluetooth should now be stopped"))

    def func_Sixpair(self):
        QtSixA_Sixpair_Window().exec_()

    def func_Force(self):
        os.system(ROOT+" sixad "+"--force "+"&")
        QtGui.QMessageBox.information(self, self.tr("QtSixA - Forced"), self.tr(""
	"The sixad driver has now been forced to start.<p>"
	"You should be able to connect your devices with no problem now<br>"
	"<i>(but please note that bluetooth is not workable for anything else in this mode, "
	"so you should stop sixad when you need to do something else)</i>"))
        #QtGui.QMessageBox.warning(self, self.tr("QtSixA - Forced"), self.tr(""
	#"sixad no longer supports this action.\nYou can still force the connection by stopping bluetooth several times before the Sixaxis/Keypad gets connected"))

    def func_Stop(self):
	if look4Root():
	    os.system(ROOT+" sixad "+"--stop")
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Stop"), self.tr("The modules have been stopped"))

    def func_ConfSixaxis(self):
        QtSixA_ConfSixaxis_Window().exec_()
	self.func_What()

    def func_ConfQtSixA(self):
        QtSixA_ConfQtSixA_Window().exec_()

    def func_ClearBT(self):
	if look4Root():
	    os.system(ROOT+" rm -rf /var/lib/bluetooth")
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Clear"), self.tr("All bluetooth data should now be cleared"))

    def func_RestoreDef(self):
	os.system("rm -rf /tmp/.sixa-notify")
	os.system("rm -rf $HOME/.config/autostart/QtSixA*tify.desktop")
	os.system("cp /usr/share/qtsixa/qtsixa.conf.bu $HOME/.qtsixa/qtsixa.conf")
        QtGui.QMessageBox.warning(self, self.tr("QtSixA - Restored"), self.tr("The default settings have been restored.<br><b>Please restart QtSixA now</b>"))

    def func_RestoreProfiles(self):
	if look4Root():
	    os.system(ROOT+" rm -rf /etc/hal/fdi/policy/DualShock3.fdi")
	    os.system(ROOT+" rm -rf /etc/hal/fdi/policy/sixa*.fdi")
	    os.system(ROOT+" cp /usr/share/qtsixa/profiles/sixa_none.fdi /etc/hal/fdi/policy/")
	    os.system(ROOT+" cp /usr/share/qtsixa/profiles.list.bu /usr/share/qtsixa/profiles.list")
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Restored"), self.tr("The default input profiles are now restored"))
	    self.func_What()

    def func_Donate(self):
	os.system("xdg-open \"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=9305140\"")

    def func_Report_Bug(self):
	os.system("xdg-open https://bugs.launchpad.net/qtsixa")

    def func_Idea(self):
	os.system("xdg-open https://blueprints.launchpad.net/qtsixa")

    def func_Questions(self):
	os.system("xdg-open https://answers.launchpad.net/qtsixa")

    def func_SourceForge(self):
	os.system("xdg-open http://qtsixa.sourceforge.net/")

    def func_UbuntuForums(self):
	os.system("xdg-open http://ubuntuforums.org/showthread.php?p=7472939")

    def func_Reference(self):
        QtSixA_Reference().exec_()

    def func_Features(self):
        self.features = open("/usr/share/qtsixa/features.html").read()
        QtGui.QMessageBox.information(self, self.tr("QtSixA - Features"), self.tr(self.features))

    def func_About_QtSixA(self):
	QtSixA_About_Window().exec_()

    def func_UpdateListOfDevices(self):
	self.Check4BluetoothDevices = commands.getoutput("hcitool con")
	self.Check4USBDevices = commands.getoutput("lsusb")

	if not "ACL" in self.Check4BluetoothDevices:
	    self.listOfDevices.item(1).setHidden(1)
            self.listOfDevices.item(2).setHidden(1)
            self.listOfDevices.item(3).setHidden(1)
            self.listOfDevices.item(4).setHidden(1)
            self.listOfDevices.item(5).setHidden(1)
            self.listOfDevices.item(6).setHidden(1)
            self.listOfDevices.item(7).setHidden(1)
            self.listOfDevices.item(8).setHidden(1)
            self.actionDiscOne.setVisible(0)
            self.actionDiscTwo.setVisible(0)
            self.actionDiscThree.setVisible(0)
            self.actionDiscFour.setVisible(0)
            self.actionDiscFive.setVisible(0)
            self.actionDiscSix.setVisible(0)
            self.actionDiscSeven.setVisible(0)
            self.actionDiscEight.setVisible(0)
            self.actionDiscNull.setVisible(1)
	    if (not "054c:03a0" in self.Check4USBDevices) and (not "054c:0306" in self.Check4USBDevices) and (not "054c:0268" in self.Check4USBDevices):
		self.listOfDevices.item(9).setHidden(1)
		self.listOfDevices.item(10).setHidden(1)
		self.listOfDevices.item(11).setHidden(1)
		self.listOfDevices.item(12).setHidden(1)
		self.listOfDevices.item(0).setHidden(0)
		self.listOfDevices.setSelectionMode(0)
	else:
	    self.func_UpdateBluetoothNames()
            self.listOfDevices.setSelectionMode(1)
	    self.listOfDevices.item(0).setHidden(1)
            self.actionDiscNull.setVisible(0)
            if (self.hidd_number_1 == ""): self.listOfDevices.item(1).setHidden(1), self.actionDiscOne.setVisible(0)
            else:
		self.listOfDevices.item(1).setText(self.hidd_number_1)
		self.listOfDevices.item(1).setHidden(0)
		self.actionDiscOne.setVisible(1)
		self.actionDiscOne.setText(self.hidd_number_1)
            if (self.hidd_number_2 == ""): self.listOfDevices.item(2).setHidden(1), self.actionDiscTwo.setVisible(0)
            else: 
		self.listOfDevices.item(2).setText(self.hidd_number_2)
		self.listOfDevices.item(2).setHidden(0)
		self.actionDiscTwo.setVisible(1)
		self.actionDiscTwo.setText(self.hidd_number_2)
	    if (self.hidd_number_3 == ""): self.listOfDevices.item(3).setHidden(1), self.actionDiscThree.setVisible(0)
            else: 
		self.listOfDevices.item(3).setText(self.hidd_number_3)
		self.listOfDevices.item(3).setHidden(0)
		self.actionDiscThree.setVisible(1)
		self.actionDiscThree.setText(self.hidd_number_3)
	    if (self.hidd_number_4 == ""): self.listOfDevices.item(4).setHidden(1), self.actionDiscFour.setVisible(0)
            else: 
		self.listOfDevices.item(4).setText(self.hidd_number_4)
		self.listOfDevices.item(4).setHidden(0)
		self.actionDiscFour.setVisible(1)
		self.actionDiscFour.setText(self.hidd_number_4)
	    if (self.hidd_number_5 == ""): self.listOfDevices.item(5).setHidden(1), self.actionDiscFive.setVisible(0)
            else: 
		self.listOfDevices.item(5).setText(self.hidd_number_5)
		self.listOfDevices.item(5).setHidden(0)
		self.actionDiscFive.setVisible(1)
		self.actionDiscFive.setText(self.hidd_number_5)
	    if (self.hidd_number_6 == ""): self.listOfDevices.item(6).setHidden(1), self.actionDiscSix.setVisible(0)
            else: 
		self.listOfDevices.item(6).setText(self.hidd_number_6)
		self.listOfDevices.item(6).setHidden(0)
		self.actionDiscSix.setVisible(1)
		self.actionDiscSix.setText(self.hidd_number_6)
	    if (self.hidd_number_7 == ""): self.listOfDevices.item(7).setHidden(1), self.actionDiscSeven.setVisible(0)
            else: 
		self.listOfDevices.item(7).setText(self.hidd_number_7)
		self.listOfDevices.item(7).setHidden(0)
		self.actionDiscSeven.setVisible(1)
		self.actionDiscSeven.setText(self.hidd_number_7)
	    if (self.hidd_number_8 == ""): self.listOfDevices.item(8).setHidden(1), self.actionDiscEight.setVisible(0)
            else: 
		self.listOfDevices.item(8).setText(self.hidd_number_8)
		self.listOfDevices.item(8).setHidden(0)
		self.actionDiscEight.setVisible(1)
		self.actionDiscEight.setText(self.hidd_number_8)

	if (not "054c:03a0" in self.Check4USBDevices) and (not "054c:0306" in self.Check4USBDevices) and (not "054c:0268" in self.Check4USBDevices):
	    self.listOfDevices.item(9).setHidden(1)
	    self.listOfDevices.item(10).setHidden(1)
	    self.listOfDevices.item(11).setHidden(1)
	    self.listOfDevices.item(12).setHidden(1)
	else:
	    self.func_UpdateUSBNames()
	    self.listOfDevices.item(0).setHidden(1)
	    self.listOfDevices.item(9).setHidden(0)
	    self.listOfDevices.item(10).setHidden(0)
	    self.listOfDevices.item(11).setHidden(0)
	    self.listOfDevices.item(12).setHidden(0)
	    self.listOfDevices.setSelectionMode(1)
	    if (self.usb_number_1 == ""): self.listOfDevices.item(9).setHidden(1)
	    else: self.listOfDevices.item(9).setText(self.usb_number_1)
	    if (self.usb_number_2 == ""): self.listOfDevices.item(10).setHidden(1)
	    else: self.listOfDevices.item(10).setText(self.usb_number_2)
	    if (self.usb_number_3 == ""): self.listOfDevices.item(11).setHidden(1)
	    else: self.listOfDevices.item(11).setText(self.usb_number_3)
	    if (self.usb_number_4 == ""): self.listOfDevices.item(12).setHidden(1)
	    else: self.listOfDevices.item(12).setText(self.usb_number_4)

	self.func_UpdateTrayTooltip()


    def func_UpdateBluetoothNames(self):
	self.hidd_number_1 = self.hidd_number_2 = self.hidd_number_3 = self.hidd_number_4 = self.hidd_number_5 = self.hidd_number_6 = self.hidd_number_7 = self.hidd_number_8 = ""
	self.nOfDevices = int(commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | grep ACL -n | tail -n 1 | awk '{printf$1}' | awk 'sub(\":\",\"\")'")) - 1
	if self.nOfDevices > 0: self.hidd_number_1 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 2 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 1: self.hidd_number_2 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 3 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 2: self.hidd_number_3 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 4 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 3: self.hidd_number_4 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 5 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 4: self.hidd_number_5 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 6 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 5: self.hidd_number_6 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 7 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 6: self.hidd_number_7 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 8 | tail -n 1 | awk '{printf$3}'")
	if self.nOfDevices > 7: self.hidd_number_8 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 9 | tail -n 1 | awk '{printf$3}'")

    def func_UpdateUSBNames(self):
	self.usb_number_1 = self.usb_number_2 = self.usb_number_3 = self.usb_number_4 = ""
	self.nOfDevices = int(commands.getoutput("echo '"+self.Check4USBDevices+"' | grep -e \"054c:03a0\" -e \"054c:0306\" -e \"054c:0268\" | grep -e \"054c:03a0\" -e \"054c:0306\" -e \"054c:0268\" -n | tail -n 1 | awk '{printf$1}' | awk 'sub(\":Bus\",\"\")'"))
	if self.nOfDevices > 0: self.usb_number_1 = commands.getoutput("echo '"+self.Check4USBDevices+"' | grep -e \"054c:03a0\" -e \"054c:0306\" -e \"054c:0268\" | awk '{printf$2\":\"$4$6}' | head -n 1 | tail -n 1")
	if self.nOfDevices > 1: self.usb_number_2 = commands.getoutput("echo '"+self.Check4USBDevices+"' | grep -e \"054c:03a0\" -e \"054c:0306\" -e \"054c:0268\" | awk '{printf$2\":\"$4$6}' | head -n 2 | tail -n 1")
	if self.nOfDevices > 2: self.usb_number_3 = commands.getoutput("echo '"+self.Check4USBDevices+"' | grep -e \"054c:03a0\" -e \"054c:0306\" -e \"054c:0268\" | awk '{printf$2\":\"$4$6}' | head -n 3 | tail -n 1")
	if self.nOfDevices > 3: self.usb_number_4 = commands.getoutput("echo '"+self.Check4USBDevices+"' | grep -e \"054c:03a0\" -e \"054c:0306\" -e \"054c:0268\" | awk '{printf$2\":\"$4$6}' | head -n 4 | tail -n 1")


    def func_UpdateDeviceStats(self):
	if self.listOfDevices.currentRow() <= 0: #No device
	    self.b_disconnect.setEnabled(0)
	    self.b_battery.setEnabled(0)
	    self.groupDevice.setEnabled(0)
	    self.groupBluetooth.setEnabled(0)
	    self.lineName.setText("")
	    self.lineType.setText("")
	    self.lineID.setText("")
	    self.lineMode.setText("")
	    self.lineAddress.setText("")
	    self.barLQ.setTextVisible(0)
	    self.barLQ.setValue(0)
	    self.barBattery.setTextVisible(0)
	    self.barBattery.setValue(0)
	    self.autoLQRefresh.stop()
	elif self.listOfDevices.currentRow() >= 9: #USB selected
	    self.parsedID = str(self.listOfDevices.item(self.listOfDevices.currentRow()).text()).split(":")
	    if (self.parsedID[2] == "054c" and self.parsedID[3] == "0268"):
		self.devName = "PLAYSTATION(R)3 Controller"
		self.devType = "Joystick"
	    elif (self.parsedID[2] == "054c" and self.parsedID[3] == "03a0"):
		self.devName = "Wireless Keypad"
		self.devType = "Keypad"
	    elif (self.parsedID[2] == "054c" and self.parsedID[3] == "0306"):
		self.devName = "PLAYSTATION(R)3 Remote"
		self.devType = "Remote"
	    else:
		self.devName = "Unkown"
		self.devType = "Unkown"
	    self.b_disconnect.setEnabled(0)
	    self.b_battery.setEnabled(0)
	    self.groupDevice.setEnabled(1)
	    self.groupBluetooth.setEnabled(0)
	    self.lineName.setText(self.devName)
	    self.lineType.setText(self.devType)
	    self.lineID.setText(self.parsedID[2]+":"+self.parsedID[3])
	    self.lineMode.setText("USB")
	    self.lineAddress.setText("")
	    self.barLQ.setTextVisible(0)
	    self.barLQ.setValue(0)
	    self.barBattery.setTextVisible(0)
	    self.barBattery.setValue(0)
	    self.autoLQRefresh.stop()
	else: #Bluetooth
	    self.devName = commands.getoutput("hcitool name "+str(self.listOfDevices.item(self.listOfDevices.currentRow()).text()))
	    if (self.devName == "PLAYSTATION(R)3 Controller"):
		self.devType = "Joystick"
		self.devID = "054c:0268"
	    elif (self.devName == "Wireless Keypad"):
		self.devType = "Keypad"
		self.devID = "054c:03a0"
	    elif (self.devName == "PLAYSTATION(R)3 Remote"):
		self.devType = "Remote"
		self.devID = "054c:0306"
	    else:
		self.devType = "Unknown"
		self.devID = "Unknown"
	    self.b_disconnect.setEnabled(1)
	    if self.devType == "Joystick": self.b_battery.setEnabled(1)
	    else: self.b_battery.setEnabled(0)
	    self.groupDevice.setEnabled(1)
	    self.groupBluetooth.setEnabled(1)
	    self.lineName.setText(self.devName)
	    self.lineType.setText(self.devType)
	    self.lineID.setText(self.devID)
	    self.lineMode.setText("Bluetooth")
	    self.lineAddress.setText(self.listOfDevices.item(self.listOfDevices.currentRow()).text())
	    self.barLQ.setTextVisible(1)
	    self.devLQ = commands.getoutput("hcitool lq "+str(self.listOfDevices.item(self.listOfDevices.currentRow()).text())+" | awk '{printf$3}'")
	    if not ("o" in self.devLQ): self.barLQ.setValue(int(self.devLQ))
	    self.label_bat.setEnabled(0)
	    self.barBattery.setEnabled(0)
	    self.barBattery.setTextVisible(0)
	    self.barBattery.setValue(0)
	    self.autoLQRefresh.start(5000)

    def func_UpdateDeviceLQ(self):
	self.devLQ = commands.getoutput("hcitool lq "+str(self.listOfDevices.item(self.listOfDevices.currentRow()).text())+" | awk '{printf$3}'")
	if not ("o" in self.devLQ): self.barLQ.setValue(int(self.devLQ))


#Advanced
    def func_Apply_signal(self):
	if (self.line_signal_bash.displayText() == ""):
	    QtGui.QMessageBox.warning(self, self.tr("QtSixA - Incomplete!"), self.tr("Something is missing...\n(Empty slot is not possible!)"))
	else:
	    if self.box_signal_disc.isChecked():
		self.signal_disc = "1"
	    else:
		self.signal_disc = "0"
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Done"), self.tr("The action has been set."))
	    os.system('sixa-lq' + ' ' + str(self.combo_signal_device.currentText()) + ' ' + str(self.combo_signal_operation.currentText()) + ' ' + str(self.spin_signal.value()) + ' ' + str(self.signal_disc) + ' ' + str(self.line_signal_bash.displayText()) + ' &' )

    def func_Reset_signal(self):
	os.system("pkill -KILL sixa-lq > /dev/null")
        QtGui.QMessageBox.information(self, self.tr("QtSixA - Stop"), self.tr("All previous set-up actions were stopped."))

    def func_Tips_signal(self):
        QtGui.QMessageBox.information(self, self.tr("QtSixA - Tips"), self.tr(""
	"Commonly used commands:"
	"<p>"
	"<b>1.</b> Any suggestions ??"
	""))

    def func_Apply_hidraw(self):
	if look4Root():
	    self.rawReport = commands.getoutput(ROOT+" /usr/sbin/sixad-raw "+str(self.combo_hidraw.currentText())+" 3654")
	    if (self.rawReport == "Found a Sixaxis"):
		os.system(ROOT+" /usr/sbin/sixad-raw "+str(self.combo_hidraw.currentText())+" &")
		QtGui.QMessageBox.information(self, self.tr("QtSixA - Done"), self.tr(""
		"The sixad driver has been initialized on the selected hidraw device sucessfully"))
	    else:
		QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error"), self.tr(""
		"The sixad driver could not start.\nAre you sure you selected a Sixaxis?"))

    def func_Refresh_signal(self):
	self.combo_signal_device.clear()
	if not "ACL" in commands.getoutput("hcitool con"):
	    self.combo_signal_device.addItem("No Sixaxis found")
	    self.combo_signal_device.setEnabled(0)
	    self.b_apply_signal.setEnabled(0)
	else:
	    self.combo_signal_device.setEnabled(1)
	    self.b_apply_signal.setEnabled(1)
	    if (self.hidd_number_1 != ""): self.combo_signal_device.addItem(self.hidd_number_1)
	    if (self.hidd_number_2 != ""): self.combo_signal_device.addItem(self.hidd_number_2)
	    if (self.hidd_number_3 != ""): self.combo_signal_device.addItem(self.hidd_number_3)
	    if (self.hidd_number_4 != ""): self.combo_signal_device.addItem(self.hidd_number_4)
	    if (self.hidd_number_5 != ""): self.combo_signal_device.addItem(self.hidd_number_5)
	    if (self.hidd_number_6 != ""): self.combo_signal_device.addItem(self.hidd_number_6)
	    if (self.hidd_number_7 != ""): self.combo_signal_device.addItem(self.hidd_number_7)
	    if (self.hidd_number_8 != ""): self.combo_signal_device.addItem(self.hidd_number_8)

    def func_Refresh_hidraw(self):
	self.combo_hidraw.clear()
	self.listofhidraws = commands.getoutput("ls /dev | awk '{printf\"/dev/\"$1\"\\n\"}' | grep hidraw").split()
	if (len(self.listofhidraws) == 0):
	    self.combo_hidraw.addItem("No hidraw devices found")
	    self.combo_hidraw.setEnabled(0)
	    self.b_apply_hidraw.setEnabled(0)
	else:
	    self.combo_hidraw.setEnabled(1)
	    self.b_apply_hidraw.setEnabled(1)
	    j = 0
	    while j < len(self.listofhidraws):
		self.combo_hidraw.addItem(self.listofhidraws[j])
		j += 1

    def func_Kill_sixadraw(self):
	if look4Root():
	    os.system(ROOT+" pkill -TERM sixad-raw > /dev/null")
	    QtGui.QMessageBox.information(self, self.tr("QtSixA - Stopped"), self.tr(""
	    "All back to normal now"))

#Systray Stuff
    def createTrayIcon(self):
	self.actionShowSixA = QtGui.QAction(self.tr("Show/Hide Main &Window"), self)
	self.connect(self.actionShowSixA, QtCore.SIGNAL("triggered()"), self.func_Show_SixA)
	self.actionCloseSystray = QtGui.QAction(QtGui.QIcon('/usr/share/qtsixa/icons/close.png'), self.tr("&Close Systray"), self)
	self.connect(self.actionCloseSystray, QtCore.SIGNAL("triggered()"), self.func_CloseSystray)

        self.trayIconMenu = QtGui.QMenu(self)
        self.trayIconMenu.addAction(self.actionAbout_QtSixA)
        self.trayIconMenu.addAction(self.actionConfSixaxis)
        self.trayIconMenu.addAction(self.actionConfQtSixA)
        self.trayIconMenu.addSeparator()
        self.trayIconMenu.addAction(self.actionShowSixA)
        self.trayIconMenu.addSeparator()
        self.trayIconMenu.addMenu(self.menuDisc)
        self.trayIconMenu.addAction(self.actionBT_Start)
        self.trayIconMenu.addAction(self.actionBT_Stop)
        self.trayIconMenu.addAction(self.actionSixpair)
        self.trayIconMenu.addAction(self.actionForce)
        self.trayIconMenu.addAction(self.actionStop)
        self.trayIconMenu.addSeparator()
        self.trayIconMenu.addAction(self.actionCloseSystray)
        #self.trayIconMenu.addAction(self.actionExit)

        self.trayIcon = QtGui.QSystemTrayIcon(self)
        self.trayIcon.setContextMenu(self.trayIconMenu)
	try:
	  self.trayIcon.activated.connect(self.func_Systray_Clicked) #Not available on some systems ( why ? )
	except:
	  print "Your system doesn't suport double-click on systray"
	  print "Your system doesn't suport disconnect devices through DBus"

	self.trayIcon.setToolTip(self.trayTooltip)
	self.trayIcon.setIcon(QtGui.QIcon('/usr/share/qtsixa/icons/qtsixa_32.png'))
	self.trayIcon.show()

    def func_Systray_Clicked(self, reason):
	if (reason == 2 or reason == 3 or reason == 4): self.func_Show_SixA()

    def func_Show_SixA(self):
	if QtSixA.isHidden(): QtSixA.show()
	else: QtSixA.hide()

    def func_CloseSystray(self):
	self.trayIcon.hide()
	self.trayIsActive = 0
	if QtSixA.isHidden(): sys.exit(0)
	else: app.setQuitOnLastWindowClosed(1)

    def func_UpdateTrayTooltip(self):
	self.trayTooltip = "<b> QtSixA 1.0.2 </b><br>"
	if (self.SixaxisProfile == "" or self.SixaxisProfile == "none" or self.SixaxisProfile == "None"): self.trayTooltip += "You're not using a Sixaxis profile"
        else: self.trayTooltip += "Your input profile is set to \"<i>"+self.SixaxisProfile+"</i>\"."
	self.trayTooltip += "<p>"

	if (self.usb_number_1 == "") and (self.hidd_number_1 == ""):
	    self.trayTooltip += "No devices found"
	else:
	    self.trayTooltip += "<u>Connected devices:</u>"
	if (self.hidd_number_1 != ""):
	    self.trayTooltip += "<br>"+self.listOfDevices.item(1).text()
	    if (self.hidd_number_2 != ""):
		self.trayTooltip += "<br>"+self.listOfDevices.item(2).text()
		if (self.hidd_number_3 != ""):
		    self.trayTooltip += "<br>"+self.listOfDevices.item(3).text()
		    if (self.hidd_number_4 != ""):
			self.trayTooltip += "<br>"+self.listOfDevices.item(4).text()
			if (self.hidd_number_5 != ""):
			    self.trayTooltip += "<br>"+self.listOfDevices.item(5).text()
			    if (self.hidd_number_6 != ""):
				self.trayTooltip += "<br>"+self.listOfDevices.item(6).text()
				if (self.hidd_number_7 != ""):
				    self.trayTooltip += "<br>"+self.listOfDevices.item(7).text()
				    if (self.hidd_number_8 != ""):
					self.trayTooltip += "<br>"+self.listOfDevices.item(8).text()
	if (self.usb_number_1 != ""):
	    self.trayTooltip += "<br>"+self.listOfDevices.item(9).text()+" (USB)"
	    if (self.usb_number_2 != ""):
		self.trayTooltip += "<br>"+self.listOfDevices.item(10).text()+" (USB)"
		if (self.usb_number_3 != ""):
		    self.trayTooltip += "<br>"+self.listOfDevices.item(11).text()+" (USB)"
		    if (self.usb_number_4 != ""):
			self.trayTooltip += "<br>"+self.listOfDevices.item(12).text()+" (USB)"
	if self.trayIsActive == 1: self.trayIcon.setToolTip(self.trayTooltip)


#Games stuff
    def func_UpdateGames(self):
	self.wEP.setVisible(0)
	self.wET.setVisible(0)
	self.wNE.setVisible(0)
	self.wSTK.setVisible(0)
	if self.listOfGames.currentText() == "ePSXe (Wine)":
	    self.wEP.setVisible(1)
	elif self.listOfGames.currentText() == "Extreme Tux Racer":
	    self.wET.setVisible(1)
	elif self.listOfGames.currentText() == "Neverball / Nevergolf":
	    self.wNE.setVisible(1)
	elif self.listOfGames.currentText() == "Super Tux Kart":
	    self.wSTK.setVisible(1)

    def func_ApplyGame(self):
	if self.listOfGames.currentText() == "ePSXe (Wine)":
	    if os.path.exists((os.getenv("HOME"))+"/.wine/user.reg"):
		self.applied = 1
		if self.radio_game_epsxe_1.isChecked():
		    os.system("regedit "+"/usr/share/qtsixa/game-profiles/wine-epsxe_axis.reg")
		elif self.radio_game_epsxe_axis.isChecked():
		    os.system("regedit "+"/usr/share/qtsixa/game-profiles/wine-epsxe_accel-mov.reg")
		else:
		    os.system("regedit "+"/usr/share/qtsixa/game-profiles/wine-epsxe_accel-driv.reg")
	    else: self.applied = 0
	elif self.listOfGames.currentText() == "Extreme Tux Racer":
	    if os.path.exists((os.getenv("HOME"))+"/.etracer/options"):
		self.applied = 1
		if self.radio_etracer_axis.isChecked():
		    content = commands.getoutput('PREV=`cat $HOME/.etracer/options | head -n 150`; MOD=`cat /usr/share/qtsixa/game-profiles/etracer_axis`; NEXT=`cat $HOME/.etracer/options | tail -n 325`; echo "$PREV $MOD $NEXT"')
		elif self.radio_etracer_accel.isChecked():
		    content = commands.getoutput('PREV=`cat $HOME/.etracer/options | head -n 150`; MOD=`cat /usr/share/qtsixa/game-profiles/etracer_accel`; NEXT=`cat $HOME/.etracer/options | tail -n 325`; echo "$PREV $MOD $NEXT"')
		else:
		    content = commands.getoutput('PREV=`cat $HOME/.etracer/options | head -n 150`; MOD=`cat /usr/share/qtsixa/game-profiles/etracer_full`; NEXT=`cat $HOME/.etracer/options | tail -n 325`; echo "$PREV $MOD $NEXT"')
		newFile = QtCore.QFile('/tmp/etracer_options')
		if not newFile.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
		    QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error!"), self.tr("Cannot write to file.\nPlease check if the location you selected is not read-only or if you enough space left on disk."))
		else:
		    newFile.writeData(content)
		    newFile.close()
		    os.system("cp /tmp/etracer_options $HOME/.etracer/options")
	    else: self.applied = 0
	elif self.listOfGames.currentText() == "Neverball / Nevergolf":
	    if os.path.exists((os.getenv("HOME"))+"/.neverball/neverballrc"):
		self.applied = 1
		if self.radio_neverball_axis.isChecked():
		    content = commands.getoutput('PREV=`cat $HOME/.neverball/neverballrc | head -n 27`; MOD=`cat /usr/share/qtsixa/game-profiles/neverballrc_axis`; NEXT=`cat $HOME/.neverball/neverballrc | tail -n 26`; echo "$PREV $MOD $NEXT"')
		else:
		    content = commands.getoutput('PREV=`cat $HOME/.neverball/neverballrc | head -n 27`; MOD=`cat /usr/share/qtsixa/game-profiles/neverballrc_accel`; NEXT=`cat $HOME/.neverball/neverballrc | tail -n 26`; echo "$PREV $MOD $NEXT"')
		newFile = QtCore.QFile('/tmp/neverballrc')
		if not newFile.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
		    QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error!"), self.tr("Cannot write to file.\nPlease check if the location you selected is not read-only or if you enough space left on disk."))
		else:
		    newFile.writeData(content)
		    newFile.close()
		    os.system("cp /tmp/neverballrc $HOME/.neverball/neverballrc")
	    else: self.applied = 0
	elif self.listOfGames.currentText() == "Super Tux Kart":
	    if os.path.exists((os.getenv("HOME"))+"/.supertuxkart/config"):
		self.applied = 1
		self.config_n_stk_1 = commands.getoutput("cat $HOME/.supertuxkart/config | grep -n \"player 1 settings\" | awk '{printf$1}' | awk 'sub(\":\",\"\")'")
		self.config_n_stk = str(  (int(self.config_n_stk_1) + 4) )
		if self.radio_stk_digital.isChecked():
		    content = commands.getoutput('PREV=`cat $HOME/.supertuxkart/config | head -n '+self.config_n_stk+'`; MOD=`cat /usr/share/qtsixa/game-profiles/stk_digital`; echo "$PREV $MOD"')
		elif self.radio_stk_axis.isChecked():
		    content = commands.getoutput('PREV=`cat $HOME/.supertuxkart/config | head -n '+self.config_n_stk+'`; MOD=`cat /usr/share/qtsixa/game-profiles/stk_axis`; echo "$PREV $MOD"')
		elif self.radio_stk_accel.isChecked():
		    content = commands.getoutput('PREV=`cat $HOME/.supertuxkart/config | head -n '+self.config_n_stk+'`; MOD=`cat /usr/share/qtsixa/game-profiles/stk_accel`; echo "$PREV $MOD"')
		else:
		    content = commands.getoutput('PREV=`cat $HOME/.supertuxkart/config | head -n '+self.config_n_stk+'`; MOD=`cat /usr/share/qtsixa/game-profiles/stk_full`; echo "$PREV $MOD"')
		newFile = QtCore.QFile('/tmp/supertuxkart_config')
		if not newFile.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
		    QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error!"), self.tr("Cannot write to file.\nPlease check if the location you selected is not read-only or if you enough space left on disk."))
		else:
		    newFile.writeData(content)
		    newFile.close()
		    os.system("cp /tmp/supertuxkart_config $HOME/.supertuxkart/config")
	    else: self.applied = 0
	else: return

        if (self.applied): self.func_Game_msg()
	else: self.func_Game_msgNO()


    def func_Game_bOn(self):
        self.b_game_apply.setEnabled(1)

    def func_Game_bOff(self):
        self.b_game_apply.setEnabled(0)

    def func_Game_msg(self):
        QtGui.QMessageBox.information(self, self.tr("QtSixA - Game Profile"), self.tr("Done!\nNow just launch the game to start the fun!"))

    def func_Game_msgNO(self):
        QtGui.QMessageBox.warning(self, self.tr("QtSixA - Game Profile"), self.tr("Done!\nThe chosen game has never been started before...\nIt's configuration file does not exist!"))

    def func_HelpGame(self):
        QtGui.QMessageBox.information(self, self.tr("QtSixA - Game Help"), self.tr(""
	"This is easy:<p>"
	"<b>1.</b> Select the game from the combo-box in the middle<br>"
	"<b>2.</b> Choose a configuration on the buttons below<br>"
	"<b>3.</b> Click the apply button on the bottom right<p>"
	"<i>Please be sure to run the game at least once before clicking \"Apply\",<br>"
	"or the profile may not be applied sucessfully</i>"))


#The Manual...
    def func_Manual(self):
        os.system("xdg-open file:///usr/share/doc/qtsixa/manual/manual_index.html")


#--------------- main ------------------
if __name__ == '__main__':

    appName     = "QtSixA"
    programName = "QtSixA"
    version     = "1.0.2"
    description = "Sixaxis Joystick Manager"
    license     = "GPL v2+"
    copyright   = "(C) 2009 falkTX"

    app = QtGui.QApplication(sys.argv)
    QtSixA = Main_QtSixA_Window()
    if config_start_minimized == "1": QtSixA.hide()
    else: QtSixA.show()
    if config_close_to_tray == "1": app.setQuitOnLastWindowClosed(0)
    sys.exit(app.exec_())
