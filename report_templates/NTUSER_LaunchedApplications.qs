function fred_report_info() {
  var info={report_cat    : "NTUSER",
            report_name   : "Launched applications",
            report_author : "Gillen Daniel",
            report_desc   : "Dump IE launched applications",
            fred_api      : 2,
            hive          : "NTUSER"
  };
  return info;
}

function IsValid(val) {
  if(typeof val !== 'undefined') return true;
  else return false;
}

function PrintTableRow(cell01,cell02,cell03) {
  println("      <tr><td style=\"white-space:nowrap\">",cell01,"</td><td style=\"padding:2px; white-space:nowrap\">",cell02,"</td><td style=\"padding:2px; white-space:nowrap\">",cell03,"</td></tr>");
}

function Rot13Decode(val) {
  var ret="";

  for(var i=0;i<val.length;i++) {
    var decoded=val.charCodeAt(i);
    if((decoded>64 && decoded<91) || (decoded>96 && decoded<123)) {
      if((decoded-13)<65 || (decoded>96 && (decoded-13)<97)) {
        decoded=(decoded-13)+26;
      } else {
        if(decoded>96 && (decoded-13)<97) {
          decoded+=13;
        } else {
          decoded-=13;
        }
      }
      ret+=String.fromCharCode(decoded);
    } else {
      ret+=val[i];
    }
  }

  return ret;
}

function PrintUserAssistEntry(key,val,os) {
  var run_count;
  var last_run;

  switch(os) {
    case "winxp":
      run_count=RegistryKeyValueToVariant(val.value,"uint32",4);

      break;
    case "win7":
      run_count=RegistryKeyValueToVariant(val.value,"uint32",4,0,1);
      last_run=RegistryKeyValueToVariant(val.value,"filetime",60);
      break;
  }

  PrintTableRow(key,run_count,last_run);
}

function fred_report_html() {
  println("  <h2>Launched applications</h2>");

  // First, we need to find the correct GUID for the current Windows version
  var path;
  var apps;
  var os;

  // Windows XP
  os="winxp";
  path="\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\{5E6AB780-7743-11CF-A12B-00AA004AE837}\\Count";
  apps=GetRegistryKeys(path);

  // TODO: Determine GUIDs for Vista / Win8

  if(!IsValid(apps)) {
    // Windows 7
    os="win7";
    path="\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\{CEBFF5CD-ACE2-4F4F-9178-9926F41749EA}\\Count";
    apps=GetRegistryKeys(path);
  }

  if(IsValid(apps)) {
    if(apps.length!=0) {
      println("  <p style=\"font-size:12\">");
      println("    <table style=\"margin-left:20px; font-size:12\">");
      println("      <tr><td><b>Application</b></td><td style=\"padding:2px\"><b>Run count</b></td><td style=\"padding:2px\"><b>Last run</b></td></tr>");

      for(var i=0;i<apps.length;i++) {
        var val=GetRegistryKeyValue(path,apps[i]);
        PrintUserAssistEntry(Rot13Decode(apps[i]),val,os);
      }

      println("    </table>");
      println("  </p>");
    } else {
      println("  <p><font color='red'>");
      println("    The list of launched applications is empty.");
      println("  </font></p>");
    }
  } else {
    println("  <p><font color='red'>");
    println("    This registry hive does not contain a list of launched applications!");
    println("  </font></p>");
  }
}
