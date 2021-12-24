function fred_report_info() {
  var info={report_cat    : "SAM",
            report_name   : "OLD - User accounts",
            report_author : "Gillen Daniel",
            report_desc   : "Dump Windows user accounts",
            fred_api      : 2,
            hive          : "SAM"
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

function print_v_info(v_key_value,info_name,str_off) {
  var offset=Number(RegistryKeyValueToVariant(v_key_value,"uint16",str_off))+0x0cc;
  var len=Number(RegistryKeyValueToVariant(v_key_value,"uint16",str_off+4));
  if(len>0) print_table_row(info_name,RegistryKeyValueToVariant(v_key_value,"utf16",offset,len));
}

function fred_report_html() {
  // See http://windowsir.blogspot.com/2006/08/getting-user-info-from-image.html
  println("  <h2>User accounts</h2>");

  // Iterate over all user names
  var user_names=GetRegistryNodes("\\SAM\\Domains\\Account\\Users\\Names");
  if(IsValid(user_names)) {
    for(var i=0;i<user_names.length;i++) {
      println("  <p style=\"font-size:12; white-space:nowrap\">");

      // Print user name
      println("    <u>",user_names[i],"</u><br />");

      println("    <table style=\"margin-left:20px; font-size:12\">");

      // Get user rid stored in "default" key
      var user_rid=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\Names\\",user_names[i]),"");
      user_rid=RegistryKeyTypeToString(user_rid.type);
      println("      <tr><td>RID:</td><td>",Number(user_rid).toString(10)," (",user_rid,")","</td></tr>");

      // RegistryKeyTypeToString returns the rid prepended with "0x". We have to remove that for further processing
      user_rid=String(user_rid).substr(2);

      // Get user's V key and print various infos
      var v_key=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"V");
      print_v_info(v_key.value,"Full name:",0x18);
      print_v_info(v_key.value,"Comment:",0x24);
      print_v_info(v_key.value,"Home directory:",0x48);
      print_v_info(v_key.value,"Home directory drive:",0x54);
      print_v_info(v_key.value,"Logon script path:",0x60);
      print_v_info(v_key.value,"Profile path:",0x6c);

      // Get user's F key and print various infos
      var f_key=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"F");
      print_table_row("Last login time:",RegistryKeyValueToVariant(f_key.value,"filetime",8));
      print_table_row("Last pw change:",RegistryKeyValueToVariant(f_key.value,"filetime",24));
      print_table_row("Last failed login:",RegistryKeyValueToVariant(f_key.value,"filetime",40));
      print_table_row("Account expires:",RegistryKeyValueToVariant(f_key.value,"filetime",32));
      print_table_row("Total logins:",RegistryKeyValueToVariant(f_key.value,"uint16",66));
      print_table_row("Failed logins:",RegistryKeyValueToVariant(f_key.value,"uint16",64));
      var acc_flags=Number(RegistryKeyValueToVariant(f_key.value,"uint16",56));
      print("      <tr><td>Account flags:</td><td>");
      if(acc_flags&0x0001) print("Disabled ");
      if(acc_flags&0x0002) print("HomeDirReq ");
      if(acc_flags&0x0004) print("PwNotReq ");
      if(acc_flags&0x0008) print("TempDupAcc ");
      // I don't think this would be useful to show
      //if(acc_flags&0x0010) print("NormUserAcc ");
      if(acc_flags&0x0020) print("MnsAcc ");
      if(acc_flags&0x0040) print("DomTrustAcc ");
      if(acc_flags&0x0080) print("WksTrustAcc ");
      if(acc_flags&0x0100) print("SrvTrustAcc ");
      if(acc_flags&0x0200) print("NoPwExpiry ");
      if(acc_flags&0x0400) print("AccAutoLock ");
      print(" (",acc_flags,")");
      println("</td></tr>");

      // Get password hint if available
      var hint=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"UserPasswordHint");
      if(typeof hint !== 'undefined') {
        // Append missing trailing utf16 zero byte
        hint.value.appendByte(0);
        hint.value.appendByte(0);
        print_table_row("Password hint:",RegistryKeyValueToVariant(hint.value,"utf16"));
      }

      // TODO: User group membership

      println("  </table>");
      println("  </p>");
    }
  } else {
    println("  <p><font color='red'>");
    println("    Unable to enumerate users!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}
