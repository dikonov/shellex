import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.shellex 1.0

import "../components"

Dialog {
  id: root
  objectName: "EditCommandPage"

  property ShellCommand command
  property ShellExecutor modeller

  property bool editAsNew: false

  allowedOrientations: Orientation.All

  SilicaFlickable {

    anchors.fill: parent

    contentHeight: content.height

    ScrollDecorator {}

    Column {
      id: content

      width: parent.width

      DialogHeader {
        title: root.editAsNew ? qsTr("New command") : qsTr("Edit command")
        acceptText: root.editAsNew ?
                      runOnCreateSwitch.checked ? qsTr("Create and run") : qsTr("Create and save")
        : qsTr("Save")
      }

        TextField {
          id: nameField
          width: parent.width
          label: acceptableInput ? qsTr("Entry name (unique)") : qsTr("Name not unique")
          placeholderText: label

          validator: CommandNameValidator {
            command: root.editAsNew ? null : root.command
            model: root.modeller.commandsModel
          }
        }

        TextArea {
          id: editField
          width: parent.width
          label: qsTr("Command to run")
          placeholderText: label

          inputMethodHints: Qt.ImhNoAutoUppercase
        }

        ComboBox {
          id: runnerChooser

          label: qsTr("Run this command")


          Component.onCompleted: {
            var currentRunner = root.command.runIn;
            if(currentRunner === ShellCommand.Fingerterm)
            {
              currentIndex = 1;
            }
            else if(currentRunner === ShellCommand.InsideApp)
            {
              currentIndex = 0;
            }
          }

          menu: ContextMenu {

            MenuItem{
              text: qsTr("inside the app")
              property int value: ShellCommand.InsideApp
            }

            MenuItem{
              enabled: modeller.fingertermInstalled
              text: qsTr("in Fingerterm")
              property int value: ShellCommand.Fingerterm
              Label {
                visible: !parent.enabled
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Fingerterm is not installed")
                font.pixelSize: Theme.fontSizeTiny
                color: Theme.secondaryColor
              }
            }

          }

        }
        TextSwitch {
          visible: root.editAsNew
          id: runOnCreateSwitch
          text: qsTr("Run on create")
          checked: true
        }

        ParameterSetup {
          id: parametersSetup
          parameters: JSON.parse(root.command.content).hasOwnProperty('parameters') ?
                        JSON.parse(root.command.content).parameters : []
        }

      }

    }

    canAccept: parametersSetup.acceptableParameters && nameField.acceptableInput

    property bool needOutputPage: root.editAsNew &&
                                  runOnCreateSwitch.checked &&
                                  runnerChooser.currentItem.value === ShellCommand.InsideApp

    acceptDestination: needOutputPage ? ( parametersSetup.count ? Qt.resolvedUrl("ProcessOutputPage.qml")
                                                                : Qt.resolvedUrl("ProcessOutputPage.qml"))
                                      : mainPage
    acceptDestinationAction: needOutputPage ? PageStackAction.Replace
                                            : PageStackAction.Pop

  onAccepted: {
    var scriptContent = JSON.stringify({script: editField.text, parameters: parametersSetup.parameters});

    console.log(scriptContent)
    if(editAsNew === true)
    {
      routineLib.createStoredCommand(nameField.text, scriptContent,
                                     "SingleLiner", runnerChooser.currentItem.value,
                                     command.output.linesMax, runOnCreateSwitch.checked);

    }
    else
    {

      root.command.content = scriptContent;
      root.command.name = nameField.text;
      root.command.runIn = runnerChooser.currentItem.value;
      commandsStore.updateCommand(root.command);

    }
  }

  Component.onCompleted: {
    nameField.text = root.command.name;
    var content = JSON.parse(root.command.content);
    editField.text = content.script;
    console.log(content.hasOwnProperty('parameters'))

  }
}
