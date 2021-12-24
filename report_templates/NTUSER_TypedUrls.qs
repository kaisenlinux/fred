function fred_report_info() {
  var info={report_cat    : "NTUSER",
            report_name   : "Typed URLs",
            report_author : "Gillen Daniel, Voncken Guy",
            report_desc   : "Dump typed URLs",
            fred_api      : 2,
            hive          : "NTUSER"
  };
  return info;
}

function IsValid(val) {
  if(typeof val !== 'undefined') return true;
  else return false;
}

function GetUrlTimestamp (url_id) {
  ret="Unknown";

  var typed_urls_time=GetRegistryKeys("\\Software\\Microsoft\\Internet Explorer\\TypedURLsTime");
  if(IsValid(typed_urls_time)) {
    for(var i=0;i<typed_urls_time.length;i++) {
      if(typed_urls_time[i]==url_id) {
        // Found
        ts=GetRegistryKeyValue("\\Software\\Microsoft\\Internet Explorer\\TypedURLsTime",typed_urls_time[i]);
        if(!IsValid(ts)) break;
        ret=RegistryKeyValueToVariant(ts.value,"filetime");
        break;
      }
    }
  }

  return ret;
}

function fred_report_html() {
  println("  <h2>Typed urls</h2>");

  // Iterate over all typed urls
  var typed_urls=GetRegistryKeys("\\Software\\Microsoft\\Internet Explorer\\TypedURLs");
  if(IsValid(typed_urls)) {
    if(typed_urls.length!=0) {
      println("  <p style=\"font-size:12\">");
      println("    <table style=\"margin-left:20px; font-size:12\">");
      println("      <tr><td><b>Last added</b></td><td><b>URL</b></td>");

      for(var i=0;i<typed_urls.length;i++) {
        var url=GetRegistryKeyValue("\\Software\\Microsoft\\Internet Explorer\\TypedURLs",typed_urls[i]);
        var ts=GetUrlTimestamp(typed_urls[i]);
        println("      <tr>");
        println("        <td style=\"white-space:nowrap\">",ts,"</td>");
        println("        <td style=\"white-space:nowrap\">",RegistryKeyValueToString(url.value,url.type),"</td>");
        println("      </tr>");
      }

      println("    </table>");
      println("  </p>");
    } else {
      println("  <p><font color='red'>");
      println("    The list of typed urls is empty.");
      println("  </font></p>");
    }
  } else {
    println("  <p><font color='red'>");
    println("    This registry hive does not contain a list of typed urls!");
    println("  </font></p>");
  }
}
