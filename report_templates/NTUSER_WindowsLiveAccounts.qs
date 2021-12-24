function fred_report_info() {
  var info={report_cat    : "NTUSER",
            report_name   : "Windows Live accounts",
            report_author : "Gillen Daniel",
            report_desc   : "Dump Windows Live accounts",
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
  println("  <h2>Windows live accounts</h2>");

  // Iterate over all contacts
  var accounts=GetRegistryKeys("\\Software\\Microsoft\\Windows Live Contacts\\Database");
  if(IsValid(accounts)) {
    println("  <p style=\"font-size:12\">");
    println("    <table style=\"margin-left:20px; font-size:12\">");

    for(var i=0;i<accounts.length;i++) {
      var val=GetRegistryKeyValue("\\Software\\Microsoft\\Windows Live Contacts\\Database",accounts[i]);
      println("      <tr><td>",accounts[i],"</td><td>",RegistryKeyValueToString(val.value,val.type),"</td></tr>");
    }
    accounts=GetRegistryKeys("\\Software\\Microsoft\\Windows Live Contacts\\Me");
    for(var i=0;i<accounts.length;i++) {
      var val=GetRegistryKeyValue("\\Software\\Microsoft\\Windows Live Contacts\\Me",accounts[i]);
      println("      <tr><td>",accounts[i],"</td><td>",RegistryKeyValueToString(val.value,val.type),"</td></tr>");
    }

    println("    </table>");
    println("  </p>");
  } else {
    println("  <p><font color='red'>");
    println("    This registry hive does not contain a list of Windows Live Accounts!");
    println("  </font></p>");
  }
}
