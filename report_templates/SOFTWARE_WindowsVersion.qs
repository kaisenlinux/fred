function fred_report_info() {
  var info={report_cat    : "SOFTWARE",
            report_name   : "Windows version",
            report_author : "Gillen Daniel",
            report_desc   : "Dump Windows version info",
            fred_api      : 2,
            hive          : "SOFTWARE"
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

function DecodeProductKey(arr) {
  //ProductKey is base24 encoded
  var keychars=new Array("B","C","D","F","G","H","J","K","M","P","Q","R","T","V","W","X","Y","2","3","4","6","7","8","9");
  var key=new Array(30);
  var ret="";
  var ncur;

  if(arr.length<66) return ret;

  arr=arr.mid(52,15);
  for(var ilbyte=24;ilbyte>=0;ilbyte--) {
    ncur=0;
    for(var ilkeybyte=14;ilkeybyte>=0;ilkeybyte--) {
      ncur=ncur*256^arr[ilkeybyte];
      arr[ilkeybyte]=ncur/24;
      ncur%=24;
    }
    ret=keychars[ncur]+ret;
    if(ilbyte%5==0 && ilbyte!=0) ret="-"+ret;
  }
  return ret;
}

function fred_report_html() {
  println("  <h2>Windows version info</h2>");

  // Windows version sp and build info
  var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","ProductName");
  if(IsValid(val)) {
    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

    print("        <tr><td>Windows version:</td><td>",RegistryKeyValueToString(val.value,val.type));
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","CSDVersion");
    if(IsValid(val)) {
      print(" ",RegistryKeyValueToString(val.value,val.type));
    }
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","CurrentBuildNumber");
    if(IsValid(val)) {
      print(" build ",RegistryKeyValueToString(val.value,val.type));
    }
    println("</td></tr>");
    // Build string
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","BuildLab");
    print_table_row("Build string:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");
    // Extended build string
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","BuildLabEx");
    print_table_row("Extended build string:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");

    // Install date
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","InstallDate");
    print_table_row("Install date:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"unixtime") : "n/a");

    // Owner and Organization info
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","RegisteredOwner");
    print_table_row("Registered owner:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","RegisteredOrganization");
    print_table_row("Registered organization:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");

    // Windows ID / Key
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","ProductId");
    print_table_row("Product ID:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","DigitalProductId");
    if(IsValid(val)) {
      var key=DecodeProductKey(val.value);
      if(key!="BBBBB-BBBBB-BBBBB-BBBBB-BBBBB") print_table_row("Product Key:",key);
      else print_table_row("Product Key:","n/a (Probably a volume license key was used)");
    } else print_table_row("Product Key:","n/a");

    // Install directory / Source directory
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","PathName");
    print_table_row("Install path:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");
    var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","SourcePath");
    print_table_row("Source path:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");

    println("    </table>");
    println("  </p>");
  } else {
    println("  <p><font color='red'>");
    println("    Unable to get product name!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}
