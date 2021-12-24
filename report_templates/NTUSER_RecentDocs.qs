function fred_report_info() {
  var info={report_cat    : "NTUSER",
            report_name   : "Recent documents",
            report_author : "Gillen Daniel",
            report_desc   : "Dump recent docs",
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
  println("  <h2>Recent documents</h2>");

  // Get list of recent docs
  var recent_docs=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs","MRUListEx");
  if(IsValid(recent_docs)) {
    // Iterate over all recent docs
    var i=0;
    var runlist=RegistryKeyValueToVariant(recent_docs.value,"uint32",i);
    if(Number(runlist)!=0xffffffff) {
      println("  <p style=\"font-size:12\">");
      println("    <table style=\"margin-left:20px; font-size:12\">");

      while(Number(runlist)!=0xffffffff) {
        var entry=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs",runlist.toString(10));
        println("      <tr><td style=\"white-space:nowrap\">",RegistryKeyValueToVariant(entry.value,"utf16",0),"</td></tr>");
        i+=4;
        runlist=RegistryKeyValueToVariant(recent_docs.value,"uint32",i);
      }

      println("    </table>");
      println("  </p>");
    } else {
      println("  <p><font color='red'>");
      println("    The list of recent documents is empty.");
      println("  </font></p>");
    }
  } else {
    println("  <p><font color='red'>");
    println("    This registry hive does not contain a list of recent documents!");
    println("  </font></p>");
  }
}
