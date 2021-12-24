function fred_report_info() {
  var info={report_cat    : "NTUSER",
            report_name   : "Windows 7 search keywords",
            report_author : "Gillen Daniel",
            report_desc   : "Dump Windows 7 search keywords",
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
  println("  <h2>Document and folder search keywords</h2>");

  // Get list of search keys
  var mrulist=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\WordWheelQuery","MRUListEx");
  if(IsValid(mrulist)) {
    // Iterate over all items
    var i=0;
    var runlist=RegistryKeyValueToVariant(mrulist.value,"uint32",i);
    if(Number(runlist)!=0xffffffff) {
      println("  <p style=\"font-size:12\">");
      println("    <table style=\"margin-left:20px; font-size:12\">");

      while(Number(runlist)!=0xffffffff) {
        var entry=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\WordWheelQuery",runlist.toString(10));
        println("      <tr><td style=\"white-space:nowrap\">",RegistryKeyValueToVariant(entry.value,"utf16",0),"</td></tr>");
        i+=4;
        runlist=RegistryKeyValueToVariant(mrulist.value,"uint32",i);
      }

      println("    </table>");
      println("  </p>");
    } else {
      println("  <p><font color='red'>");
      println("    The list of document and search keywords is empty.");
      println("  </font></p>");
    }
  } else {
    println("  <p><font color='red'>");
    println("    This registry hive does not contain a list of document and folder search keywords!");
    println("  </font></p>");
  }
}
