module.exports = [
  {
    "type": "heading",
    "defaultValue": "Helium Configuration"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Time Display"
      },
      {
        "type": "text",
        "defaultValue": "Turning on the following setting will cause the hour to be shown on the innermost ring and the minute to be shown on the outermost ring."
      },
      {
        "type": "toggle",
        "messageKey": "MinuteOnOut",
        "label": "Show Minute on Outermost ring",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "More Settings"
      },
      {
        "type": "toggle",
        "messageKey": "ShowBattery",
        "label": "Show Battery Status",
        "defaultValue": true
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
