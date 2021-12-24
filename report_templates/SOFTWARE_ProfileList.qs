function fred_report_info() {
  var info={report_cat    : "SOFTWARE",
            report_name   : "Profile list",
            report_author : "Gillen Daniel",
            report_desc   : "Dump profile list",
            fred_api      : 2,
            hive          : "SOFTWARE"
  };
  return info;
}

var table_style = "border-collapse:collapse; margin-left:20px; font-family:arial; font-size:12";
var cell_style  = "border:1px solid #888888; padding:5; white-space:nowrap;";

function IsValid(val) {
  return (typeof val!=='undefined');
}

function PrintTableHeaderCell(str) {
  println("        <th style=\"",cell_style,"\">",str,"</th>");
}

function PrintTableDataCell(alignment,str) {
  var style=cell_style+" text-align:"+alignment+";";
  println("        <td style=\"",style,"\">",str,"</td>");
}

function fred_report_html() {
  var val;

  println("  <h2>Profile List</h2>");

  var profile_list=GetRegistryNodes("\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList");
  if(IsValid(profile_list) && profile_list.length>0) {
    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <table style=\""+table_style+"\">");

    println("      <tr>");
    PrintTableHeaderCell("Profile ID");
    PrintTableHeaderCell("Last load time");
    PrintTableHeaderCell("Image path");
    println("      </tr>");

    for(var i=0;i<profile_list.length;i++) {
      // Get profile image path
      val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\"+profile_list[i],"ProfileImagePath");
      var image_path=IsValid(val) ? RegistryKeyValueToString(val.value,val.type) : "n/a";

      // Get last load time (Saved as 2 dwords. Another "good" idea of M$ ;-))
      var loadtime_low=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\"+profile_list[i],"ProfileLoadTimeLow");
      var loadtime_high=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\"+profile_list[i],"ProfileLoadTimeHigh");
      var load_time=(IsValid(loadtime_low) && IsValid(loadtime_high)) ? RegistryKeyValueToVariant(loadtime_low.value.append(loadtime_high.value),"filetime",0) : "n/a";

      // TODO: There is more to decode under \\Microsoft\\Windows NT\\CurrentVersion\\ProfileList

      println("      <tr>");
      PrintTableDataCell("left",profile_list[i]);
      PrintTableDataCell("left",load_time);
      PrintTableDataCell("left",image_path);
      println("      </tr>");
    }

    println("    </table>");
    println("  </p>");
  } else {
    println("    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None");
  }
}
