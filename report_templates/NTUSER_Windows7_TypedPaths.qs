function fred_report_info() {
  var info={report_cat    : "NTUSER",
            report_name   : "Windows 7 typed paths",
            report_author : "Gillen Daniel",
            report_desc   : "Dump Windows 7 typed paths",
            fred_api      : 2,
            hive          : "NTUSER"
  };
  return info;
}

function IsValid(val) {
  if(typeof val !== 'undefined') return true;
  else return false;
}

function fred_report_html() {
  println("  <h2>Typed paths</h2>");

  // Iterate over all typed paths
  var urls=GetRegistryKeys("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\TypedPaths");
  if(IsValid(urls)) {
    if(urls.length!=0) {
      println("  <p style=\"font-size:12\">");
      println("    <table style=\"margin-left:20px; font-size:12\">");

      for(var i=0;i<urls.length;i++) {
        var val=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\TypedPaths",urls[i]);
        println("      <tr><td style=\"white-space:nowrap\">",RegistryKeyValueToString(val.value,val.type),"</td></tr>");
      }

      println("    </table>");
      println("  </p>");
    } else {
      println("  <p><font color='red'>");
      println("    The list of typed paths is empty.");
      println("  </font></p>");
    }
  } else {
    println("  <p><font color='red'>");
    println("    This registry hive does not contain a list of typed paths!");
    println("  </font></p>");
  }
}
