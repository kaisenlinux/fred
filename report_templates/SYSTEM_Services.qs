function fred_report_info() {
  var info={report_cat    : "SYSTEM",
            report_name   : "Services",
            report_author : "Gillen Daniel",
            report_desc   : "Dump services",
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

function ZeroPad(number,padlen) {
  var ret=number.toString(10);
  if(!padlen || ret.length>=padlen) return ret;
  return Math.pow(10,padlen-ret.length).toString().slice(1)+ret;
}

function PrintTableRow(cell01,cell02,cell03,cell04,cell05) {
  println("      <tr>");
  PrintTableDataCell("left",cell01);
  PrintTableDataCell("left",cell02);
  PrintTableDataCell("left",cell03);
  PrintTableDataCell("left",cell04);
  PrintTableDataCell("left",cell05);
  println("      </tr>");
}

function ListService(service_node) {
  // Service name
  var name=GetRegistryKeyValue(service_node,"DisplayName");
  name=(IsValid(name)) ? RegistryKeyValueToString(name.value,name.type) : "Unknwon";
  // Service group
  var group=GetRegistryKeyValue(service_node,"Group");
  group=(IsValid(group)) ? RegistryKeyValueToString(group.value,group.type) : "";
  // Service exe
  var image=GetRegistryKeyValue(service_node,"ImagePath");
  image=(IsValid(image)) ? RegistryKeyValueToString(image.value,image.type) : "Unknwon";
  // Start
  var start=GetRegistryKeyValue(service_node,"Start");
  start=(IsValid(start)) ? RegistryKeyValueToString(start.value,start.type) : -1;
  switch(Number(start)) {
    case 0:
      start="Boot";
      break;
    case 1:
      start="System";
      break;
    case 2:
      start="Automatic";
      break;
    case 3:
      start="Manual";
      break;
    case 4:
      start="Disabled";
      break;
    default:
      start="Unknown";
  }
  // Description
  var desc=GetRegistryKeyValue(service_node,"Description");
  desc=(IsValid(desc)) ? RegistryKeyValueToString(desc.value,desc.type) : "";
  
  PrintTableRow(name,group,start,image,desc)
}

function fred_report_html() {
  var val;

  println("  <h2>Services</h2>");

  // Get current controlset
  var cur_controlset=GetRegistryKeyValue("\\Select","Current");
  if(IsValid(cur_controlset)) {
    cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
    // Current holds a DWORD value, thus we get a string like 0x00000000, but
    // control sets are referenced by its decimal representation.
    cur_controlset="ControlSet"+ZeroPad(parseInt(String(cur_controlset).substr(2,8),16),3)

    // Get list of possible services
    var services=GetRegistryNodes(cur_controlset+"\\Services");
    if(IsValid(services)) {
      println("  <p style=\"font-size:12; white-space:nowrap\">");
      println("    <table style=\""+table_style+"\">");
      println("      <tr>");
      PrintTableHeaderCell("Name");
      PrintTableHeaderCell("Group");
      PrintTableHeaderCell("Startup");
      PrintTableHeaderCell("Image path");
      PrintTableHeaderCell("Description");
      println("      </tr>");

      for(var i=0;i<services.length;i++) {
        // Get service type
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+services[i],"Type");
        if(!IsValid(val)) continue;
        val=RegistryKeyValueToString(val.value,val.type);
        if(Number(val)!=16 && Number(val)!=32) continue;
        ListService(cur_controlset+"\\Services\\"+services[i]);
      }

      println("    </table>");
      println("  </p>");
    } else {
      println("  <p><font color='red'>");
      println("    This registry hive does not contain any services!<br />");
      println("  </font></p>");
    }
  } else {
    println("  <p><font color='red'>");
    println("    Unable to determine current control set!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}
