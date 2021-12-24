function fred_report_info() {
  var info={report_cat    : "SYSTEM",
            report_name   : "System time info",
            report_author : "Gillen Daniel",
            report_desc   : "Dump system time info",
            fred_api      : 2,
            hive          : "SYSTEM"
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

function ToUTC(num) {
  var retnum=new Number(num);
  if(retnum&0x80000000) {
    retnum=((0xFFFFFFFF-retnum)+1)/60;
    return "UTC+"+Number(retnum).toString(10);
  } else {
    retnum=retnum/60;
    if(retnum!=0) return "UTC-"+Number(retnum).toString(10);
    else return "UTC+"+Number(retnum).toString(10);
  }
}

function ZeroPad(number,padlen) {
  var ret=number.toString(10);
  if(!padlen || ret.length>=padlen) return ret;
  return Math.pow(10,padlen-ret.length).toString().slice(1)+ret;
}

function fred_report_html() {
  var val;

  println("  <h2>System time info</h2>");

  // Get current controlset
  var cur_controlset=GetRegistryKeyValue("\\Select","Current");
  if(IsValid(cur_controlset)) {
    cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
    // Current holds a DWORD value, thus we get a string like 0x00000000, but
    // control sets are referenced by its decimal representation.
    cur_controlset="ControlSet"+ZeroPad(parseInt(String(cur_controlset).substr(2,8),16),3)

    // Get W32Time service settings
    var w32time_startup_method="n/a";
    var w32time_time_servers="n/a";
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\W32Time","Start");
    if(IsValid(val)) {
      val=RegistryKeyValueToString(val.value,val.type);
      switch(Number(val)) {
        case 0:
          w32time_startup_method="Boot";
          break;
        case 1:
          w32time_startup_method="System";
          break;
        case 2:
          w32time_startup_method="Automatic";
          break;
        case 3:
          w32time_startup_method="Manual";
          break;
        case 4:
          w32time_startup_method="Disabled";
          break;
        default:
          w32time_startup_method="Unknown";
      }
      // If service is enabled, get ntp server
      if(Number(val)<4) {
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\W32Time\\Parameters","NtpServer");
        if(IsValid(val)) w32time_time_servers=RegistryKeyValueToString(val.value,val.type);
      }
    }

    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <table style=\""+table_style+"\">");
    println("      <tr><td>Active control set:</td><td>",cur_controlset,"</td></tr>");
    println("      <tr><td>W32Time startup method:</td><td>",w32time_startup_method,"</td></tr>");
    println("      <tr><td>W32Time NTP servers:</td><td>",w32time_time_servers,"</td></tr>");
    println("    </table>");
    println("    <br />");
    println("    <table style=\""+table_style+"\">");
    println("      <tr>");
    PrintTableHeaderCell("Setting name");
    PrintTableHeaderCell("Time zone");
    println("      </tr>");

    // Active time bias
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","ActiveTimeBias");
    var active_bias=(IsValid(val)) ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a"

    // Std. tz name and bias
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","StandardName");
    var std_name=(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a";
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","StandardBias");
    var std_bias=(IsValid(val)) ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a";

    // Daylight tz name and bias
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","DaylightName");
    var daylight_name=(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a";
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","DaylightBias");
    var daylight_bias=(IsValid(val)) ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a";

    println("      <tr>");
    PrintTableDataCell("left","Active");
    PrintTableDataCell("left",active_bias);
    println("      </tr>");
    println("      <tr>");
    PrintTableDataCell("left","Standard");
    PrintTableDataCell("left",std_bias+" ("+std_name+")");
    println("      </tr>");
    println("      <tr>");
    PrintTableDataCell("left","Daylight");
    PrintTableDataCell("left",daylight_bias+" ("+daylight_name+")");
    println("      </tr>");

    println("    </table>");
    println("  </p>");
  } else {
    println("  <p><font color='red'>");
    println("    Unable to determine current control set!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}
