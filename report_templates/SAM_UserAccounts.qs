function fred_report_info() {
  var info={report_cat    : "SAM",
            report_name   : "User accounts",
            report_author : "Gillen Daniel, Voncken Guy",
            report_desc   : "Dump Windows user accounts",
            fred_api      : 2,
            hive          : "SAM"
  };
  return info;
}

var table_style = "border-collapse:collapse; margin-left:20px; font-family:arial; font-size:12;";
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


function Get_v_info(v_key_value,str_off) {
  var ret_str="";
  var offset=Number(RegistryKeyValueToVariant(v_key_value,"uint16",str_off))+0x0cc;
  var len=Number(RegistryKeyValueToVariant(v_key_value,"uint16",str_off+4));
  if(len>0) ret_str=RegistryKeyValueToVariant(v_key_value,"utf16",offset,len)

  return ret_str;
}

function fred_report_html() {
  // See http://windowsir.blogspot.com/2006/08/getting-user-info-from-image.html
  println("  <h2>User accounts</h2>");

  // Iterate over all user names
  var user_names=GetRegistryNodes("\\SAM\\Domains\\Account\\Users\\Names");
  if(IsValid(user_names)) {
    println("  <table style=\""+table_style+"\">");

    println("    <tr>");
    PrintTableHeaderCell("Name");
    PrintTableHeaderCell("RID");
    PrintTableHeaderCell("Full<br>name");
    PrintTableHeaderCell("Last<br>login");
    PrintTableHeaderCell("Last PW<br>change");
    PrintTableHeaderCell("Last failed<br>login");
    PrintTableHeaderCell("Account<br>expiry");
    PrintTableHeaderCell("Total<br>logins");
    PrintTableHeaderCell("Failed<br>logins");
    PrintTableHeaderCell("Flags");
    PrintTableHeaderCell("Password<br>hint");
    PrintTableHeaderCell("Home drive<br>and dir");
    PrintTableHeaderCell("Logon<br>script path");
    PrintTableHeaderCell("Profile<br>path");
    PrintTableHeaderCell("Comment");
    println("    </tr>");

    for(var i=0;i<user_names.length;i++) {
      // Get user rid stored in "default" key
      var user_rid=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\Names\\",user_names[i]),"");  
      user_rid=RegistryKeyTypeToString(user_rid.type);
      user_rid_dec=Number(user_rid).toString(10);

      // Get user's V key and print various infos
      user_rid=String(user_rid).substr(2);   // Remove "0x"
      var v_key=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"V");
      var full_name=Get_v_info(v_key.value,0x18);
      var comment=Get_v_info(v_key.value,0x24);
      var home_dir=Get_v_info(v_key.value,0x48);
      var home_dir_drive=Get_v_info(v_key.value,0x54);
      var logon_script_path=Get_v_info(v_key.value,0x60);
      var profile_path=Get_v_info(v_key.value,0x6c);

      // Get user's F key and print various infos
      var f_key=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"F");
      var last_login_time=RegistryKeyValueToVariant(f_key.value,"filetime",8);
      var last_pw_change=RegistryKeyValueToVariant(f_key.value,"filetime",24);
      var last_failed_login=RegistryKeyValueToVariant(f_key.value,"filetime",40);
      var account_expires=RegistryKeyValueToVariant(f_key.value,"filetime",32);
      var total_logins=RegistryKeyValueToVariant(f_key.value,"uint16",66);
      var failed_logins=RegistryKeyValueToVariant(f_key.value,"uint16",64);

      var acc_flags=Number(RegistryKeyValueToVariant(f_key.value,"uint16",56));
      var acc_flags_str="";
      if(acc_flags&0x0200) acc_flags_str+="NoPwExpiry ";
      if(acc_flags&0x0001) acc_flags_str+="Disabled ";
      if(acc_flags&0x0004) acc_flags_str+="PwNotReq ";
      if(acc_flags&0x0002) acc_flags_str+="HomeDirReq ";
      if(acc_flags&0x0008) acc_flags_str+="TempDupAcc ";
//      if(acc_flags&0x0010) acc_flags_str+="NormUserAcc ";  // I don't think this would be useful to show
      if(acc_flags&0x0020) acc_flags_str+="MnsAcc ";
      if(acc_flags&0x0040) acc_flags_str+="DomTrustAcc ";
      if(acc_flags&0x0080) acc_flags_str+="WksTrustAcc ";
      if(acc_flags&0x0100) acc_flags_str+="SrvTrustAcc ";
      if(acc_flags&0x0400) acc_flags_str+="AccAutoLock ";

      // Get password hint if available
      var hint=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"UserPasswordHint");
      if(IsValid(hint)) {
        // Append missing trailing utf16 zero byte
        hint.value.appendByte(0);
        hint.value.appendByte(0);
        hint=RegistryKeyValueToVariant(hint.value,"utf16");
      } else {
        hint="";
      }

      // TODO: User group membership

      println ("    <tr>");
      PrintTableDataCell("left",user_names[i]);
      PrintTableDataCell("right",String(user_rid_dec)+" (0x"+user_rid+")");
      PrintTableDataCell("left",full_name);
      PrintTableDataCell("right",last_login_time);
      PrintTableDataCell("right",last_pw_change);
      PrintTableDataCell("right",last_failed_login);
      PrintTableDataCell("left",account_expires);
      PrintTableDataCell("right",total_logins);
      PrintTableDataCell("right",failed_logins);
      PrintTableDataCell("left",acc_flags_str);
      PrintTableDataCell("left",hint);
      PrintTableDataCell("left",home_dir_drive+" "+home_dir);
      PrintTableDataCell("left",logon_script_path);
      PrintTableDataCell("left",profile_path);
      PrintTableDataCell("left",comment);

      println ("    </tr>")
    }
    println("  </table>");
    println("</p>");
  } else {
    println("<p><font color='red'>");
    println("  Unable to enumerate users!<br />");
    println("  Are you sure you are running this report against the correct registry hive?");
    println("</font></p>");
  }
}
