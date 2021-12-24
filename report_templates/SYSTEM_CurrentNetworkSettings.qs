function fred_report_info() {
  var info={report_cat    : "SYSTEM",
            report_name   : "Current network settings",
            report_author : "Gillen Daniel",
            report_desc   : "Dump current network settings",
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

function fred_report_html() {
  // See Appendix A: TCP/IP Configuration Parameters:
  // http://technet.microsoft.com/de-de/library/cc739819%28v=WS.10%29.aspx
  var val;

  println("  <h2>Current network settings (Tcp/Ip)</h2>");

  // Get current controlset
  var cur_controlset=GetRegistryKeyValue("\\Select","Current");
  if(IsValid(cur_controlset)) {
    cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
    // Current holds a DWORD value, thus we get a string like 0x00000000, but
    // control sets are referenced by its decimal representation.
    cur_controlset="ControlSet"+ZeroPad(parseInt(String(cur_controlset).substr(2,8),16),3)

    // Computer name
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\ComputerName\\ComputerName","ComputerName");

    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <table style=\""+table_style+"\">");
    println("      <tr><td>Active control set:</td><td>",cur_controlset,"</td></tr>");
    println("      <tr><td>Computer name:</td><td>",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "","</td></tr>");
    println("    </table>");
    println("    <br />");
    println("    <table style=\""+table_style+"\">");
    println("      <tr>");
    PrintTableHeaderCell("Adapter");
    PrintTableHeaderCell("Configuration");
    PrintTableHeaderCell("IP address");
    PrintTableHeaderCell("Subnet mask");
    PrintTableHeaderCell("Nameserver(s)");
    PrintTableHeaderCell("Domain");
    PrintTableHeaderCell("Default gateway");
    PrintTableHeaderCell("DHCP server");
    PrintTableHeaderCell("DHCP lease optained");
    PrintTableHeaderCell("DHCP lease terminates");
    println("      </tr>");

    // Iterate over all available network adapters
    var adapters=GetRegistryNodes(cur_controlset+"\\Services\\Tcpip\\Parameters\\Adapters");
    for(var i=0;i<adapters.length;i++) {
      // Try to get a human readable name
      // According to http://technet.microsoft.com/de-de/library/cc780532%28v=ws.10%29.aspx
      // the {4D36E972-E325-11CE-BFC1-08002BE10318} key name might be (and hopefully is) static :)
      val=GetRegistryKeyValue(cur_controlset+"\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\"+adapters[i]+"\\Connection","Name");
      var adapter_name=IsValid(val) ? RegistryKeyValueToString(val.value,val.type) : adapters[i];

      // Get settings node
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\Tcpip\\Parameters\\Adapters\\"+adapters[i],"IpConfig");
      var adapter_settings_node=RegistryKeyValueToVariant(val.value,"utf16",0);

      // Get configuration mode
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"EnableDHCP");
      var dhcp_enabled=Number(RegistryKeyValueToString(val.value,val.type));

      var ip_address="";
      var subnet_mask="";
      var nameservers="";
      var domain="";
      var default_gateway="";
      var dhcp_server="";
      var lease_obtained="";
      var lease_terminates="";

      if(dhcp_enabled) {
        // DHCP server
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpServer");
        dhcp_server=(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "";
        // IP address
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpIPAddress");
        ip_address=(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "";
        // Subnet mask
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpSubnetMask");
        subnet_mask=(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "";
        // Nameserver(s)
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpNameServer");
        nameservers=(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "";
        // Domain
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpDomain");
        domain=(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "";
        // Default gw
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpDefaultGateway");
        default_gateway=(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "";
        // Lease obtained
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"LeaseObtainedTime");
        lease_obtained=(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"unixtime",0) : "";
        // Lease valid until
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"LeaseTerminatesTime");
        lease_terminates=(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"unixtime",0) : "";
      } else {
        // IP address
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"IPAddress");
        ip_address=(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "";
        // Subnet mask
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"SubnetMask");
        subnet_mask=(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "";
        // Nameserver
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"NameServer");
        nameservers=(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "";
        // Domain
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"Domain");
        domain=(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "";
        // Default gw
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DefaultGateway");
        default_gateway=(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "";
        dhcp_server="n/a";
        lease_obtained="n/a";
        lease_terminates="n/a";
      }

      println("      <tr>");
      PrintTableDataCell("left",adapter_name);
      PrintTableDataCell("left",dhcp_enabled ? "DHCP" : "Static");
      PrintTableDataCell("left",ip_address);
      PrintTableDataCell("left",subnet_mask);
      PrintTableDataCell("left",nameservers);
      PrintTableDataCell("left",domain);
      PrintTableDataCell("left",default_gateway);
      PrintTableDataCell("left",dhcp_server);
      PrintTableDataCell("left",lease_obtained);
      PrintTableDataCell("left",lease_terminates);
      println("      </tr>");

      // TODO: Check for EnableSecurityFilters, TCPAllowedPorts and UDPAllowedPorts to get firewall status.

      // TODO: Get persistent routes from \ControlSet001\Services\Tcpip\Parameters\PersistentRoutes
    }
    println("    </table>");
    println("  </p>");
  } else {
    println("  <p><font color='red'>");
    println("    Unable to determine current control set!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}
