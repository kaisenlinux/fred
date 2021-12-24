function fred_report_info() {
  var info={report_cat    : "NTUSER",
            report_name   : "Autoruns",
            report_author : "Gillen Daniel",
            report_desc   : "Dump autorun keys",
            fred_api      : 2,
            hive          : "NTUSER"
  };
  return info;
}

function IsValid(val) {
  if(typeof val !== 'undefined') return true;
  else return false;
}

function print_table_row(cell01,cell02) {
  println("      <tr><td>",cell01,"</td><td>",cell02,"</td></tr>");
}

function ListAutoruns(autorun_path,autorun_key) {
  println("  <p style=\"font-size:12; white-space:nowrap\">");
  println("    <u>"+autorun_key+"</u><br />");
  var run_keys=GetRegistryKeys(autorun_path+autorun_key);
  if(IsValid(run_keys) && run_keys.length>0) {
    println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");
    print_table_row("<b>Name</b>","<b>Executable</b>");

    for(var i=0;i<run_keys.length;i++) {
      var val=GetRegistryKeyValue(autorun_path+autorun_key,run_keys[i]);
      print_table_row(run_keys[i],RegistryKeyValueToString(val.value,val.type));
    }
    
    println("    </table>");
  } else {
    println("    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None");
  }
  println("  </p>");
}

function fred_report_html() {
  var val;

  println("  <h2>User Autoruns</h2>");

  // Run
  ListAutoruns("\\Microsoft\\Windows\\CurrentVersion\\","Run");

  // RunOnce
  ListAutoruns("\\Microsoft\\Windows\\CurrentVersion\\","RunOnce");

  // RunOnceEx
  ListAutoruns("\\Microsoft\\Windows\\CurrentVersion\\","RunOnceEx");

  // TODO: There might be a Run under WindowsNT\CurrentVersion\Run too!
}
