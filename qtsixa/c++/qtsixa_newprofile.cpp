#include <QtGui>

#include "qtsixa_newprofile.h"
#include "qtsixa_keylist.h"

QtSixA_NewProfile::QtSixA_NewProfile(QDialog *parent)
    : QDialog(parent)
{
    ui_newprofile.setupUi(this);

    connect(ui_newprofile.b_keys, SIGNAL(clicked()), this, SLOT(func_PKE()));
    connect(ui_newprofile.b_tips, SIGNAL(clicked()), this, SLOT(func_Tips()));
    connect(ui_newprofile.b_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_newprofile.b_done, SIGNAL(clicked()), this, SLOT(func_Done()));
    connect(ui_newprofile.combo_left, SIGNAL(currentIndexChanged(QString)), this, SLOT(func_UpdateComboLeft()));
    connect(ui_newprofile.combo_right, SIGNAL(currentIndexChanged(QString)), this, SLOT(func_UpdateComboRight()));

    ui_newprofile.group_left_h->setEnabled(0);
    ui_newprofile.group_left_v->setEnabled(0);
    ui_newprofile.group_right_h->setEnabled(0);
    ui_newprofile.group_right_v->setEnabled(0);

}

void QtSixA_NewProfile::func_PKE()
{
    QtSixA_Keylist().exec();
}

void QtSixA_NewProfile::func_Tips()
{
    QMessageBox::information(this, tr("QtSixA - Tips && Tricks"), tr(""
          "<body style=\"font-size:10pt;\">Some tips you may need:<p><body style=\"font-size:8pt;\">"
          "<b>1. </b>Click on the \"View available keys\" buttons to see a full list of available keys you can assign to a button;<br>"
          "<b>2. </b>Write the key you want to a button input-text;<br>"
          "<b>3. </b>If you don't want a button to work as key set the input-text to \"none\" (without quotes);<br>"
          "<b>4. </b>Don't forget about the 'Axis' part!<br>"
          "<b>5. </b>Combinations are possible using '+' between key names (maximum 4 keys, Meta not supported)<br>"
          "<b>6. </b>It's also possible to assign a mouse-button to joystick-button, use \"mouse_BUTTON\" on those you want that. (BUTTON is a number)<br>"
          "<b>7. </b>Once you finish, you can open the generated file for a further customization."));
}

void QtSixA_NewProfile::func_UpdateComboLeft()
{
    if (ui_newprofile.combo_left->currentIndex() == 2) //Custom Buttons
    {
        ui_newprofile.group_left_h->setEnabled(1);
        ui_newprofile.group_left_v->setEnabled(1);
    } else {
        ui_newprofile.group_left_h->setEnabled(0);
        ui_newprofile.group_left_v->setEnabled(0);
    }
}

void QtSixA_NewProfile::func_UpdateComboRight()
{
    if (ui_newprofile.combo_right->currentIndex() == 2) //Custom Buttons
    {
        ui_newprofile.group_right_h->setEnabled(1);
        ui_newprofile.group_right_v->setEnabled(1);
    } else {
        ui_newprofile.group_right_h->setEnabled(0);
        ui_newprofile.group_right_v->setEnabled(0);
    }
}

void QtSixA_NewProfile::func_Warning()
{
    QMessageBox::warning(this, tr("QtSixA - Incomplete!"), tr("Something is missing...\n(Empty slot is not possible!)"));
}

void QtSixA_NewProfile::func_Done()
{
    return;
    //      if (self.line_app.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_author.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_l2.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_l1.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_r2.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_r1.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_select.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_start.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_up.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_down.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_left.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_right.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_square.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_triangle.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_circle.displayText() == ""):
    //          self.func_Warning()
    //      elif (self.line_cross.displayText() == ""):
    //          self.func_Warning()
    //
    //      elif (self.combo_left.currentIndex() == 2): #Custom Buttons
    //        if (self.line_hls_left.displayText() == ""):
    //          self.func_Warning()
    //        elif (self.line_hls_right.displayText() == ""):
    //          self.func_Warning()
    //        elif (self.line_vls_up.displayText() == ""):
    //          self.func_Warning()
    //        elif (self.line_vls_down.displayText() == ""):
    //          self.func_Warning()
    //        elif (self.combo_right.currentIndex() == 2): #Custom Buttons
    //          if (self.line_hrs_left.displayText() == ""):
    //            self.func_Warning()
    //          elif (self.line_hrs_right.displayText() == ""):
    //            self.func_Warning()
    //          elif (self.line_vrs_up.displayText() == ""):
    //            self.func_Warning()
    //          elif (self.line_vrs_down.displayText() == ""):
    //            self.func_Warning()
    //          else:
    //            self.func_ExportToFDI()
    //        else:
    //          self.func_ExportToFDI()
    //
    //      elif (self.combo_right.currentIndex() == 2): #Custom Buttons
    //        if (self.line_hrs_left.displayText() == ""):
    //          self.func_Warning()
    //        elif (self.line_hrs_right.displayText() == ""):
    //          self.func_Warning()
    //        elif (self.line_vrs_up.displayText() == ""):
    //          self.func_Warning()
    //        elif (self.line_vrs_down.displayText() == ""):
    //          self.func_Warning()
    //        elif (self.combo_left.currentIndex() == 2): #Custom Buttons
    //          if (self.line_hls_left.displayText() == ""):
    //            self.func_Warning()
    //          elif (self.line_hls_right.displayText() == ""):
    //            self.func_Warning()
    //          elif (self.line_vls_up.displayText() == ""):
    //            self.func_Warning()
    //          elif (self.line_vls_down.displayText() == ""):
    //            self.func_Warning()
    //          else:
    //            self.func_ExportToFDI()
    //        else:
    //          self.func_ExportToFDI()
    //
    //      else:
    //        self.func_ExportToFDI()
}

void QtSixA_NewProfile::func_ExportToFDI()
{
      return;
      //      if (self.combo_left.currentIndex() == 3): #None
      //        self.ComboLeftText_H = "mode=none"
      //        self.ComboLeftText_V = "mode=none"
      //      elif (self.combo_left.currentIndex() == 0): #Mouse
      //        self.ComboLeftText_H = "mode=relative axis=+2x deadzone=5000"
      //        self.ComboLeftText_V = "mode=relative axis=+2y deadzone=5000"
      //      elif (self.combo_left.currentIndex() == 1): #Scroll
      //        self.ComboLeftText_H = "mode=relative axis=+1zx deadzone=7500"
      //        self.ComboLeftText_V = "mode=relative axis=+1zy deadzone=7500"
      //      elif (self.combo_left.currentIndex() == 2): #Custom Buttons
      //        self.ComboLeftText_H = "mode=accelerated keylow="+self.line_hls_left.text()+" keyhigh="+self.line_hls_right.text()+" deadzone=15000"
      //        self.ComboLeftText_V = "mode=accelerated keylow="+self.line_vls_up.text()+" keyhigh="+self.line_vls_down.text()+" deadzone=15000"
      //
      //      if (self.combo_right.currentIndex() == 3): #None
      //        self.ComboRightText_H = "mode=none"
      //        self.ComboRightText_V = "mode=none"
      //      elif (self.combo_right.currentIndex() == 0): #Mouse
      //        self.ComboRightText_H = "mode=relative axis=+2x deadzone=5000"
      //        self.ComboRightText_V = "mode=relative axis=+2y deadzone=5000"
      //      elif (self.combo_right.currentIndex() == 1): #Scroll
      //        self.ComboRightText_H = "mode=relative axis=+1zx deadzone=7500"
      //        self.ComboRightText_V = "mode=relative axis=+1zy deadzone=7500"
      //      elif (self.combo_right.currentIndex() == 2): #Custom Buttons
      //        self.ComboRightText_H = "mode=accelerated keylow="+self.line_hrs_left.text()+" keyhigh="+self.line_hrs_right.text()+" deadzone=7500"
      //        self.ComboRightText_V = "mode=accelerated keylow="+self.line_vrs_up.text()+" keyhigh="+self.line_vrs_down.text()+" deadzone=7500"
      //
      //      self.FDI_Content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n<deviceinfo version=\"0.2\">\n  <device>\n    <match key=\"info.capabilities\" contains=\"input\">\n      <!-- Sixaxis configuration for \""+self.line_app.text()+"\", by \""+self.line_author.text()+"\" -->\n      <match key=\"input.product\" contains=\"PLAYSTATION(R)3 Controller\">\n        <merge key=\"input.x11_driver\" type=\"string\">joystick</merge>\n\n        <merge key=\"input.x11_options.MapAxis1\" type=\"string\">"+self.ComboLeftText_H+"</merge>\n        <merge key=\"input.x11_options.MapAxis2\" type=\"string\">"+self.ComboLeftText_V+"</merge>\n        <merge key=\"input.x11_options.MapAxis3\" type=\"string\">"+self.ComboRightText_H+"</merge>\n        <merge key=\"input.x11_options.MapAxis4\" type=\"string\">"+self.ComboRightText_V+"</merge>\n	<merge key=\"input.x11_options.MapAxis5\" type=\"string\">mode=none</merge>\n	<merge key=\"input.x11_options.MapAxis6\" type=\"string\">mode=none</merge>\n	<merge key=\"input.x11_options.MapAxis7\" type=\"string\">mode=none</merge>\n\n        <merge key=\"input.x11_options.MapButton1\" type=\"string\">key="+self.line_select.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton2\" type=\"string\">none</merge>\n        <merge key=\"input.x11_options.MapButton3\" type=\"string\">none</merge>\n        <merge key=\"input.x11_options.MapButton4\" type=\"string\">key="+self.line_start.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton17\" type=\"string\">key="+self.line_ps.text()+"</merge>\n\n        <merge key=\"input.x11_options.MapButton5\" type=\"string\">key="+self.line_up.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton8\" type=\"string\">key="+self.line_left.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton6\" type=\"string\">key="+self.line_right.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton7\" type=\"string\">key="+self.line_down.text()+"</merge>\n\n        <merge key=\"input.x11_options.MapButton10\" type=\"string\">key="+self.line_r2.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton9\" type=\"string\">key="+self.line_l2.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton12\" type=\"string\">key="+self.line_r1.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton11\" type=\"string\">key="+self.line_l1.text()+"</merge>\n\n        <merge key=\"input.x11_options.MapButton15\" type=\"string\">key="+self.line_cross.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton14\" type=\"string\">key="+self.line_circle.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton16\" type=\"string\">key="+self.line_square.text()+"</merge>\n        <merge key=\"input.x11_options.MapButton13\" type=\"string\">key="+self.line_triangle.text()+"</merge>\n\n      </match>\n    </match>\n  </device>\n</deviceinfo>\n"
      //
      //      self.FDI_SelectedFileLocation = QtGui.QFileDialog.getSaveFileName(self)
      //      if self.FDI_SelectedFileLocation.isEmpty():
      //          pass
      //      else:
      //        #newFile = QtCore.QFile(self.FDI_SelectedFileLocation)
      //        #if not newFile.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
      //          #QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error!"), self.tr("Cannot write to file.\nPlease check if the location you selected is not read-only or if you enough space left on disk."))
      //        #else:
      //          self.sixadFileW = open((str(self.FDI_SelectedFileLocation)+".fdi"), "w")
      //          self.sixadFileW.write(self.FDI_Content)
      //          self.sixadFileW.close()
      //          os.system('sed -e "s/key=mouse_/button=/" -i /'+str(self.FDI_SelectedFileLocation+'.fdi'))
      //          os.system('sed -e "s/key=Mouse_/button=/" -i /'+str(self.FDI_SelectedFileLocation+'.fdi'))
      //          os.system('sed -e "s/key=MOUSE_/button=/" -i /'+str(self.FDI_SelectedFileLocation+'.fdi'))
      //          os.system('sed -e "s/key=none/none/" -i /'+str(self.FDI_SelectedFileLocation+'.fdi'))
      //          os.system('sed -e "s/key=None/none/" -i /'+str(self.FDI_SelectedFileLocation+'.fdi'))
      //          os.system('sed -e "s/key=NONE/none/" -i /'+str(self.FDI_SelectedFileLocation+'.fdi'))
      //          QtGui.QMessageBox.information(self, self.tr("QtSixA - Done!"), self.tr("It's done!\nA new profile has been saved.\n \nTo add the new profile, use the \"Add Profile\" button.\nFeel free to quit now"))
}
