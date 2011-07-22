#include <QtGui>

#include "qtsixa_sixaxis.h"
#include "qtsixa_addprofile.h"
#include "qtsixa_newprofile.h"

QtSixA_Configure_Sixaxis::QtSixA_Configure_Sixaxis(QDialog *parent)
    : QDialog(parent)
{
    ui_sixaxis.setupUi(this);

//#Read sixad configuration file
//sixad_file = commands.getoutput(". /etc/default/sixad; echo $Enable_leds $LED_js_n $LED_n $LED_plus $LED_anim $Enable_buttons $Enable_sbuttons $Enable_axis $Enable_accel $Enable_accon $Enable_speed $Enable_pos $Enable_rumble $Legacy $Debug").split()
//sixad_config_leds = sixad_file[0]
//sixad_config_led_js_n = sixad_file[1]
//sixad_config_led_n = sixad_file[2]
//sixad_config_led_plus = sixad_file[3]
//sixad_config_led_anim = sixad_file[4]
//sixad_config_buttons = sixad_file[5]
//sixad_config_sbuttons = sixad_file[6]
//sixad_config_axis = sixad_file[7]
//sixad_config_accel = sixad_file[8]
//sixad_config_accon = sixad_file[9]
//sixad_config_speed = sixad_file[10]
//sixad_config_pos = sixad_file[11]
//sixad_config_rumble = sixad_file[12]
//sixad_config_legacy = sixad_file[13]
//sixad_config_debug = sixad_file[14]
//

    i_saw_the_warning = 0;
    applied2profile = 0;
    applied2override = 0;
    applied2sixad = 0;
    applied2boot = 0;
    applied2lr3 = 0;
    applied2uinput = 0;
    Clicked = 0;

    hidd_number_1 = "";
    hidd_number_2 = "";
    hidd_number_3 = "";
    hidd_number_4 = "";
    hidd_number_5 = "";
    hidd_number_6 = "";
    hidd_number_7 = "";
    hidd_number_8 = "";
    nOfDevices = 0;

//self.Check4BluetoothDevices = commands.getoutput("hcitool con")
//self.nOfDevices_str = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | grep ACL -n | tail -n 1 | awk '{printf$1}' | awk 'sub(\":\",\"\")'")
//if self.nOfDevices_str != "": self.nOfDevices = int(self.nOfDevices_str) - 1
//if self.nOfDevices > 0: self.hidd_number_1 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 2 | tail -n 1 | awk '{printf$3}'")
//if self.nOfDevices > 1: self.hidd_number_2 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 3 | tail -n 1 | awk '{printf$3}'")
//if self.nOfDevices > 2: self.hidd_number_3 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 4 | tail -n 1 | awk '{printf$3}'")
//if self.nOfDevices > 3: self.hidd_number_4 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 5 | tail -n 1 | awk '{printf$3}'")
//if self.nOfDevices > 4: self.hidd_number_5 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 6 | tail -n 1 | awk '{printf$3}'")
//if self.nOfDevices > 5: self.hidd_number_6 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 7 | tail -n 1 | awk '{printf$3}'")
//if self.nOfDevices > 6: self.hidd_number_7 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 8 | tail -n 1 | awk '{printf$3}'")
//if self.nOfDevices > 7: self.hidd_number_8 = commands.getoutput("echo '"+self.Check4BluetoothDevices+"' | head -n 9 | tail -n 1 | awk '{printf$3}'")
//
//if (self.hidd_number_1 == ""):
//    self.applied2override_forced = 1
//    self.group_specific.setEnabled(0)
//    self.combo_specific_device.addItem("No Sixaxis found")
//else:
//    self.applied2override_forced = 0
//    self.group_specific.setEnabled(1)
//    if (self.hidd_number_1 != "" and commands.getoutput("hcitool name "+self.hidd_number_1+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_1)
//    if (self.hidd_number_2 != "" and commands.getoutput("hcitool name "+self.hidd_number_2+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_2)
//    if (self.hidd_number_3 != "" and commands.getoutput("hcitool name "+self.hidd_number_3+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_3)
//    if (self.hidd_number_4 != "" and commands.getoutput("hcitool name "+self.hidd_number_4+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_4)
//    if (self.hidd_number_5 != "" and commands.getoutput("hcitool name "+self.hidd_number_5+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_5)
//    if (self.hidd_number_6 != "" and commands.getoutput("hcitool name "+self.hidd_number_6+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_6)
//    if (self.hidd_number_7 != "" and commands.getoutput("hcitool name "+self.hidd_number_7+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_7)
//    if (self.hidd_number_8 != "" and commands.getoutput("hcitool name "+self.hidd_number_8+" | grep 'PLAYSTATION(R)3 Controller'") ): self.combo_specific_device.addItem(self.hidd_number_8)
//
//self.overridesN = commands.getoutput("ls /etc/hal/fdi/policy/ | grep x11-qtsixa | awk 'sub(\"x11-qtsixa_\",\"\")' | awk 'sub(\".fdi\",\"\")' | awk 'sub(\"_\",\" -> \")'")
//if (self.overridesN == ""): self.group_specific.setChecked(0)
//else: self.group_specific.setChecked(1)
//
//if (int(sixad_config_leds) == 0):
//    self.groupLED.setChecked(0)
//    #self.checkLED_plus.setChecked(0)
//    #self.checkLED_anim.setChecked(0)
//else:
//    self.groupLED.setChecked(1)
//    self.spinLED.setValue(int(sixad_config_led_n))
//    if (int(sixad_config_led_plus)): self.checkLED_plus.setChecked(1)
//    if (int(sixad_config_led_anim)): self.checkLED_anim.setChecked(1)
//if (int(sixad_config_buttons)): self.checkButtons.setChecked(1)
//if (int(sixad_config_sbuttons)): self.checkSButtons.setChecked(1)
//if (int(sixad_config_axis)): self.checkAxis.setChecked(1)
//if (int(sixad_config_accel)): self.checkAccel.setChecked(1)
//if (int(sixad_config_accon)): self.checkAccon.setChecked(1)
//if (int(sixad_config_speed)): self.checkSpeed.setChecked(1)
//if (int(sixad_config_pos)): self.checkPos.setChecked(1)
//if (int(sixad_config_rumble)): self.checkRumble.setChecked(1)
//if (int(sixad_config_led_js_n)):
//    self.optLEDn.setChecked(1)
//    self.frameLED.setEnabled(0)
//else:
//    self.optLEDm.setChecked(1)
//if (int(sixad_config_legacy)):
//    self.checkLegacy.setChecked(1)
//    self.groupInput.setEnabled(0)
//    self.optLEDn.setEnabled(0)
//    self.optLEDm.setChecked(1)
//if (int(sixad_config_debug)): self.checkDebug.setChecked(1)
//
//if os.path.exists('/etc/arch-release'):
//    if (commands.getoutput("cat /etc/rc.conf | grep sixad") != ""): self.checkBoot.setChecked(1)
//elif os.path.exists('/etc/rc2.d/S90sixad'): self.checkBoot.setChecked(1)
//
//if commands.getoutput("cat /etc/modules | grep uinput") != "": self.checkUInput.setChecked(1)
//
//i = 0
//while i < len(listOfSixaxisProfiles):
//    if i % 2 == 0:
//      self.inputComboBox.addItem(listOfSixaxisProfiles[i+1])
//      self.combo_specific_profiles.addItem(listOfSixaxisProfiles[i+1])
//    i += 1
//
//self.current_profile_first = commands.getoutput("ls /etc/hal/fdi/policy | grep sixa | awk 'sub(\"sixa_\",\"\")' | awk 'sub(\".fdi\",\"\")'")
//
//if (self.current_profile_first == ""): self.current_profile = "none"
//else: self.current_profile = self.current_profile_first.split()[0]
//
//if (self.current_profile == "none"):
//  self.fdiProfile = "none"
//  self.inputBox.setChecked(0)
//  self.sPic.setPixmap(QtGui.QPixmap('/usr/share/qtsixa/pics/sixa_none.png'))
//else:
//  if (self.current_profile in listOfSixaxisProfiles):
//    x = listOfSixaxisProfiles.index(self.current_profile)
//    self.inputComboBox.setCurrentIndex(((x/2)+1))
//    self.fdiProfile = listOfSixaxisProfiles[x]
//    self.sPic.setPixmap(QtGui.QPixmap('/usr/share/qtsixa/pics/sixa_'+self.fdiProfile+'.png'))
//  else:
//    self.fdiProfile = "none"
//    self.inputComboBox.setCurrentIndex(0)
//
//if ("MapButton2" in commands.getoutput("cat /usr/share/qtsixa/sixaxis-profiles/sixa_2h4u.fdi | grep disable-mouse")):
//    self.checkLR3.setChecked(1)
//
//#if QtCore.PYQT_VERSION < 0x040500: self.b_new.setEnabled(0)
//
//self.connect(self.b_overrides, QtCore.SIGNAL("clicked()"), self.func_ShowOverrides)
//self.connect(self.b_apply, QtCore.SIGNAL('clicked()'), self.func_Apply)
//self.connect(self.b_ok, QtCore.SIGNAL('clicked()'), self.func_OK)
//self.connect(self.optLEDn, QtCore.SIGNAL("clicked()"), self.func_LED_js)
//self.connect(self.optLEDm, QtCore.SIGNAL("clicked()"), self.func_LED_man)
//self.connect(self.optLEDn, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.optLEDm, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkLED_plus, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkLED_anim, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkButtons, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkSButtons, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkAxis, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkAccel, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkAccon, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkSpeed, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkPos, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkRumble, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkLegacy, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkLegacy, QtCore.SIGNAL("clicked()"), partial(self.func_Legacy_Check))
//self.connect(self.spinLED, QtCore.SIGNAL("valueChanged(int)"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.groupLED, QtCore.SIGNAL("clicked()"), self.func_GroupLED)
//self.connect(self.checkDebug, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "sixad"))
//self.connect(self.checkLR3, QtCore.SIGNAL("clicked()"), partial(self.func_Apply_Enable, "lr3"))

    connect(ui_sixaxis.b_new, SIGNAL(clicked()), this, SLOT(func_NewProfile()));
    connect(ui_sixaxis.b_add, SIGNAL(clicked()), this, SLOT(func_AddProfile()));

//self.connect(self.inputComboBox, QtCore.SIGNAL('currentIndexChanged(QString)'), self.func_UpdateProfile)
//self.connect(self.inputComboBox, QtCore.SIGNAL('currentIndexChanged(QString)'), partial(self.func_Apply_Enable, "profile"))
//self.connect(self.inputBox, QtCore.SIGNAL('clicked()'), self.func_ClickBoxOnProfile)
//self.connect(self.inputBox, QtCore.SIGNAL('clicked()'), partial(self.func_Apply_Enable, "profile"))
//self.connect(self.checkBoot, QtCore.SIGNAL('clicked()'), partial(self.func_Apply_Enable, "boot"))
//self.connect(self.checkUInput, QtCore.SIGNAL('clicked()'), partial(self.func_Apply_Enable, "uinput"))
//self.connect(self.group_specific, QtCore.SIGNAL('clicked()'), partial(self.func_Apply_Enable, "override"))
//self.connect(self.combo_specific_device, QtCore.SIGNAL('currentIndexChanged(QString)'), partial(self.func_Apply_Enable, "override"))
//self.connect(self.combo_specific_profiles, QtCore.SIGNAL('currentIndexChanged(QString)'), partial(self.func_Apply_Enable, "override"))
//
}

//
//def func_ShowOverrides(self):
//    self.overrides = commands.getoutput("ls /etc/hal/fdi/policy/ | grep x11-qtsixa | awk 'sub(\"x11-qtsixa_\",\"\")' | awk 'sub(\".fdi\",\"\")' | awk 'sub(\"_\",\" -> \")'")
//    if (self.overrides == ""):
//      QtGui.QMessageBox.information(self, self.tr("QtSixA - Show"), self.tr("No current overrides are set."))
//    else:
//      QtGui.QMessageBox.information(self, self.tr("QtSixA - Show"), self.tr("Current overrides:\n \n"+self.overrides+"\n"))
//
//def func_Apply_Enable(self, what2apply):
//    self.b_apply.setEnabled(1)
//    self.b_cancel.setEnabled(1)
//    if (what2apply == "sixad"): self.applied2sixad = 1
//    elif (what2apply == "profile"): self.applied2profile = 1
//    elif (what2apply == "override"): self.applied2override = 1
//    elif (what2apply == "boot"): self.applied2boot = 1
//    elif (what2apply == "lr3"): self.applied2lr3 = 1
//    elif (what2apply == "uinput"): self.applied2uinput = 1
//
//def func_Legacy_Check(self):
//    if self.checkLegacy.isChecked():
//        self.groupInput.setEnabled(0)
//        self.optLEDn.setEnabled(0)
//        self.optLEDm.setChecked(1)
//        self.frameLED.setEnabled(1)
//    else:
//        self.groupInput.setEnabled(1)
//        self.optLEDn.setEnabled(1)
//
//def func_LED_man(self):
//    self.frameLED.setEnabled(1)
//
//def func_LED_js(self):
//    self.frameLED.setEnabled(0)
//
//def func_OK(self):
//    self.func_Apply()
//    self.close()
//
//def func_Apply(self):
//    if self.applied2override == 1 and self.applied2override_forced == 0: #overrides
//        if self.group_specific.isChecked():
//            self.specificDevice = self.combo_specific_device.currentText()
//            self.specificProfile_str = self.combo_specific_profiles.currentIndex()
//            if (self.specificProfile_str == 0):
//                self.specificProfile = "none"
//            else:
//                if (self.specificProfile_str in listOfSixaxisProfiles):
//                    xxx = listOfSixaxisProfiles.index(self.specificProfile_str)
//                    self.specificProfile = listOfSixaxisProfiles[xxx-1]
//                else:
//                    QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error"), self.tr("The selected profile is not on the list of profiles!\nQtSixA will quit now!"))
//                    self.close()
//                    exit(-1)
//            if look4Root():
//                os.system(ROOT+" cp /usr/share/qtsixa/sixaxis-profiles/sixa_"+str(self.specificProfile)+".fdi /etc/hal/fdi/policy/x11-qtsixa_"+str(self.specificDevice)+"_"+str(self.specificProfile)+".fdi")
//                os.system(ROOT+' sed -e "s/ contains/ string_outof/" -i /etc/hal/fdi/policy/x11-qtsixa_'+str(self.specificDevice)+'_'+str(self.specificProfile)+'.fdi')
//                os.system(ROOT+' sed -e "s/PLAYSTATION(R)3 Controller/PLAYSTATION(R)3 Controller ('+str(self.specificDevice)+')/" -i /etc/hal/fdi/policy/x11-qtsixa_'+str(self.specificDevice)+'_'+str(self.specificProfile)+'.fdi')
//                QtGui.QMessageBox.information(self, self.tr("QtSixA - Done"), self.tr("The override has been set:\n \nDevice: %1\nProfile: \"%2\"\n").arg(self.specificDevice).arg(self.specificProfile_str))
//        else:
//            if look4Root(): os.system(ROOT+" rm -rf /etc/hal/fdi/policy/x11-qtsixa_*.fdi")
//
//    if self.applied2profile == 1: #Sixaxis profile stuff
//        if look4Root():
//            os.system(ROOT+" rm -rf /etc/hal/fdi/policy/sixa*.fdi")
//            os.system(ROOT+" cp /usr/share/qtsixa/sixaxis-profiles/sixa_"+self.fdiProfile+".fdi "+"/etc/hal/fdi/policy/")
//            QtGui.QMessageBox.information(self, self.tr("QtSixA - Profile"), self.tr("Input profile has now been set to \"%1\".\n \nIf you want to use it now, please disconnect your Sixaxis\nand connect them again.").arg(self.inputComboBox.currentText()))
//
//
//    if self.applied2boot == 1: #Enable/Disable at boot
//        if look4Root():
//            if self.checkBoot.isChecked(): os.system(ROOT+" sixad "+"--boot-yes")
//            else: os.system(ROOT+" sixad "+"--boot-no")
//
//
//    if self.applied2lr3 == 1: #Enable/Disable LR3
//        if look4Root():
//            if self.checkLR3.isChecked():
//                os.system(ROOT+" sed \"s/MapButton2\\\" type=\\\"string\\\">none</MapButton2\\\" type=\\\"string\\\">disable-mouse</\" -i /etc/hal/fdi/policy/sixa_*.fdi")
//                os.system(ROOT+" sed \"s/MapButton3\\\" type=\\\"string\\\">none</MapButton3\\\" type=\\\"string\\\">disable-keys</\" -i /etc/hal/fdi/policy/sixa_*.fdi")
//                os.system(ROOT+" sed \"s/MapButton2\\\" type=\\\"string\\\">none</MapButton2\\\" type=\\\"string\\\">disable-mouse</\" -i /usr/share/qtsixa/sixaxis-profiles/sixa_*.fdi")
//                os.system(ROOT+" sed \"s/MapButton3\\\" type=\\\"string\\\">none</MapButton3\\\" type=\\\"string\\\">disable-keys</\" -i /usr/share/qtsixa/sixaxis-profiles/sixa_*.fdi")
//            else:
//                os.system(ROOT+" sed \"s/MapButton2\\\" type=\\\"string\\\">disable-mouse</MapButton2\\\" type=\\\"string\\\">none</\" -i /etc/hal/fdi/policy/sixa_*.fdi")
//                os.system(ROOT+" sed \"s/MapButton3\\\" type=\\\"string\\\">disable-keys<</MapButton3\\\" type=\\\"string\\\">none</\" -i /etc/hal/fdi/policy/sixa_*.fdi")
//                os.system(ROOT+" sed \"s/MapButton2\\\" type=\\\"string\\\">disable-mouse</MapButton2\\\" type=\\\"string\\\">none</\" -i /usr/share/qtsixa/sixaxis-profiles/sixa_*.fdi")
//                os.system(ROOT+" sed \"s/MapButton3\\\" type=\\\"string\\\">disable-keys<</MapButton3\\\" type=\\\"string\\\">none</\" -i /usr/share/qtsixa/sixaxis-profiles/sixa_*.fdi")
//            QtGui.QMessageBox.information(self, self.tr("QtSixA - Profile"), self.tr("The chosen Sixaxis profile has now L3/R3 settings\nIf you want to use them now, please disconnect your Sixaxis\nand connect them again."))
//
//
//    if self.applied2uinput == 1: #Enable/Disable uinput at boot
//        if look4Root():
//            if self.checkUInput.isChecked():
//                if commands.getoutput("cat /etc/modules | grep uinput") == "": #only if it isn't there yet
//                    os.system("cp /etc/modules /tmp/modules.sixad")
//                    os.system("echo 'uinput' >> /tmp/modules.sixad")
//                    os.system(ROOT+" cp /tmp/modules.sixad /etc/modules")
//            else:
//                os.system(ROOT+" sed \"s/uinput//\" -i /etc/modules")
//
//
//    if self.applied2sixad == 1: #sixad settings
//#starts here----------------------------
//        if self.groupLED.isChecked(): self.txtLEDs = "1"
//        else: self.txtLEDs = "0"
//        if self.optLEDn.isChecked(): self.txtLED_js = "1"
//        else: self.txtLED_js = "0"
//        self.txtLED_n = str(self.spinLED.text())
//        if self.checkLED_plus.isChecked(): self.txtLED_plus = "1"
//        else: self.txtLED_plus = "0"
//        if self.checkLED_anim.isChecked(): self.txtLED_anim = "1"
//        else: self.txtLED_anim = "0"
//        if self.checkButtons.isChecked(): self.txtEnable_buttons = "1"
//        else: self.txtEnable_buttons = "0"
//        if self.checkSButtons.isChecked(): self.txtEnable_sbuttons = "1"
//        else: self.txtEnable_sbuttons = "0"
//        if self.checkAxis.isChecked(): self.txtEnable_axis = "1"
//        else: self.txtEnable_axis = "0"
//        if self.checkAccel.isChecked(): self.txtEnable_accel = "1"
//        else: self.txtEnable_accel = "0"
//        if self.checkAccon.isChecked(): self.txtEnable_accon = "1"
//        else: self.txtEnable_accon = "0"
//        if self.checkSpeed.isChecked(): self.txtEnable_speed = "1"
//        else: self.txtEnable_speed = "0"
//        if self.checkPos.isChecked(): self.txtEnable_pos = "1"
//        else: self.txtEnable_pos = "0"
//        if self.checkRumble.isChecked(): self.txtEnable_rumble = "1"
//        else: self.txtEnable_rumble = "0"
//        if self.checkLegacy.isChecked(): self.txtEnable_legacy = "1"
//        else: self.txtEnable_legacy = "0"
//        if self.checkDebug.isChecked(): self.txtEnable_debug = "1"
//        else: self.txtEnable_debug = "0"
//
//        self.sixadFile = (""
//        "# sixad configuration file\n"
//        "#\n"
//        "# Please use \"1\" or \"0\" instead of \"yes/true\" or \"no/false\"\n"
//        "#  (this values are sent to C code...)\n"
//        "\n"
//        "\n"
//        "# Enable LEDs?\n"
//        "Enable_leds="+self.txtLED_n+"\n"
//        "\n"
//        "# Use js # for LED #? (overrides \"LED_n\" and \"LED_plus\"; doesn't work with \"Legacy\" )\n"
//        "LED_js_n="+self.txtLED_js+"\n"
//        "\n"
//        "# Start with LED #\n"
//        "LED_n="+self.txtLED_n+"\n"
//        "\n"
//        "# LED # increase after new connection?\n"
//        "LED_plus="+self.txtLED_plus+"\n"
//        "\n"
//        "# Enable LEDs/Rumble animation?\n"
//        "LED_anim="+self.txtLED_anim+"\n"
//        "\n"
//        "# Enable buttons?\n"
//        "Enable_buttons="+self.txtEnable_buttons+"\n"
//        "\n"
//        "# Enable sensible buttons? (as axis)\n"
//        "Enable_sbuttons="+self.txtEnable_sbuttons+"\n"
//        "\n"
//        "# Enable axis? (Left && Right)\n"
//        "Enable_axis="+self.txtEnable_axis+"\n"
//        "\n"
//        "# Enable accelerometers?\n"
//        "Enable_accel="+self.txtEnable_accel+"\n"
//        "\n"
//        "# Enable acceleration?\n"
//        "Enable_accon="+self.txtEnable_accon+"\n"
//        "\n"
//        "# Enable speed?\n"
//        "Enable_speed="+self.txtEnable_speed+"\n"
//        "\n"
//        "# Enable position?\n"
//        "Enable_pos="+self.txtEnable_pos+"\n"
//        "\n"
//        "# Enable rumble? (Incomplete)\n"
//        "Enable_rumble="+self.txtEnable_rumble+"\n"
//        "\n"
//        "# Use Old/Legacy driver instead of sixad (Workaround for PowerPC[32] UInput)\n"
//        "Legacy="+self.txtEnable_legacy+"\n"
//        "\n"
//        "# Enable debug?\n"
//        "Debug="+self.txtEnable_debug+"\n"
//        "")
//
//        self.sixadFileW = open("/tmp/sixad", "w")
//        self.sixadFileW.write(self.sixadFile)
//        self.sixadFileW.close()
//        if look4Root(): os.system(ROOT+" cp /tmp/sixad /etc/default/sixad")
//
//        if (config_show_warnings == "yes"):
//          #QtGui.QMessageBox.information(self, self.tr("QtSixA - Done"), self.tr(""
//          #"You have changed sixad settings, which will only be applied on the next connected Sixaxis"))
//          if commands.getoutput("uname -m") == "powerpc" and not self.checkLegacy.isChecked():
//              QtGui.QMessageBox.warning(self, self.tr("QtSixA - Warning"), self.tr(""
//              "These Sixaxis settings are invalid!<br>"
//              "<b>Please enable \"Legacy Driver\" when running from a 32-bit PowerPC system!</b>"))
//#ends here--------------------------------------------------------------------
//
//    self.applied2profile = 0
//    self.applied2override = 0
//    self.applied2sixad = 0
//    self.applied2boot = 0
//    self.applied2lr3 = 0
//    self.b_cancel.setEnabled(0)
//    self.b_apply.setEnabled(0)
//
//def func_GroupLED(self):
//    if self.groupLED.isChecked():
//        self.checkLED_plus.setEnabled(1)
//        self.checkLED_anim.setEnabled(1)
//    else:
//        self.checkLED_plus.setEnabled(0)
//        self.checkLED_anim.setEnabled(0)
//    self.func_Apply_Enable("sixad")
//
void QtSixA_Configure_Sixaxis::func_NewProfile()
{
    QtSixA_AddProfile().exec();
}

void QtSixA_Configure_Sixaxis::func_AddProfile()
{
    QtSixA_NewProfile().exec();
}

//def func_ClickBoxOnProfile(self):
//    if (self.Clicked == "0"):
//        self.Clicked = "1"
//        self.fdiProfile = "none"
//        self.inputComboBox.setCurrentIndex(0)
//    else:
//        self.Clicked = "0"
//        self.inputComboBox.setCurrentIndex(0)
//
//def func_UpdateProfile(self):
//    if (self.inputComboBox.currentIndex() == 0):
//        self.sPic.setPixmap(QtGui.QPixmap('/usr/share/qtsixa/pics/sixa_none.png'))
//        self.fdiProfile = "none"
//    else:
//        if (self.inputComboBox.currentText() in listOfSixaxisProfiles):
//            x = listOfSixaxisProfiles.index(self.inputComboBox.currentText())
//            self.fdiProfile = listOfSixaxisProfiles[x-1]
//            self.sPic.setPixmap(QtGui.QPixmap('/usr/share/qtsixa/pics/sixa_'+self.fdiProfile+'.png'))
//        else:
//            self.fdiProfile = "none"
//            self.inputComboBox.setCurrentIndex(0)
//
