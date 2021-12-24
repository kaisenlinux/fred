// Internet Download Manager is a program that may integrate itself into all commpon browsers.
// It claims to download much faster and be more comfortable.
//
// The code below has been written after analysis of several systems where IDM was installed.
// Documentation also can be found under
//    https://tise2015.kku.ac.th/drupal/sites/default/files/1-s2.0-S1742287610000575-main.pdf
// That documentation refers too a much older version of IDM, but the string obfuscation 
// technique is still the same.

function fred_report_info()
{
   var info = { 
                 report_cat    : "NTUSER",
                 report_name   : "Internet Download Manager",
                 report_author : "Guy Voncken <develop@faert.net>",
                 report_desc   : "Extract IDM download list and configuration settings",
                 fred_api      : 2,
                 hive          : "NTUSER"
              };
  return info;
}

function IsValid (val)
{
  if(typeof val !== 'undefined') return true;
  else return false;
}


function GetKeyVal(path, key)
{
   var val=GetRegistryKeyValue(path, key);
   var Ret = (IsValid(val)) ? RegistryKeyValueToString (val.value, val.type) : "";
   return String (Ret);
}



var table_style = "border-collapse:collapse; margin-left:20px; font-family:arial; font-size:14;";
var cell_style  = "border:1px solid #888888; padding:5; max-width:500px; word-wrap: break-word; ";


function PrintTableHeaderCell (str)
{
   println ("        <th style=\"",cell_style,"\">",str,"</th>");
}

function PrintTableDataCell(alignment,str)
{
   var style = cell_style + "text-align:" + alignment + ";";
   println ("        <td style=\"",style,"\">",str,"</td>");
}

function StrSizeToNum (StrSize)
{
   var Str = String(StrSize);
   var Num = 0;
   var i   = Str.length-1;

   while (i >= 1)
   {
      if (Str[i] == " ")
      {
         i--;
      }
      else
      {
         Byte = 16 * parseInt (Str[i-1], 16) + 
                     parseInt (Str[i  ], 16);
         Num = Num * 256 + Byte;
         i -= 2;
      }
   }
   return Num;
}

function SizeToHuman (StrSize)
{
   var Size = parseInt (StrSize,10);
   var Unit = "&nbsp;&nbsp;B";

   if (Size > 1024) {Size = Size / 1024.0; Unit = "KiB"};
   if (Size > 1024) {Size = Size / 1024.0; Unit = "MiB"};
   if (Size > 1024) {Size = Size / 1024.0; Unit = "GiB"};
   if (Size > 1024) {Size = Size / 1024.0; Unit = "TiB"};
   
   return Size.toFixed(2) + " " + Unit;
}

function ConvertSillyDateFormat (DateIn)
{
   var DateOut = DateIn.slice (16,20) + " " + DateIn.slice (0,6) + "&nbsp&nbsp;" + DateIn.slice (7,15);
   return DateOut;
}

function ConvString8 (EncStr, Encrypted)
{
   var Str = "";
   for (i=0; i<EncStr.length; i+=3)
   {
      var N1 = parseInt (EncStr[i]  , 16);
      var N0 = parseInt (EncStr[i+1], 16);
      if (Encrypted)
         N0 = N0 ^ 0x0F;   // Not really encrypted, but obfuscated
      var Code = N1*16 + N0;
      if (Code > 0)
         Str = Str + String.fromCharCode (Code);
   }
   return Str;
}

function ConvString16 (EncStr, Encrypted)
{
   var Str = "";
   for (i=0; i<EncStr.length; i+=6)
   {
      var N1 = parseInt (EncStr[i]  , 16);
      var N0 = parseInt (EncStr[i+1], 16);
      var N3 = parseInt (EncStr[i+3], 16);
      var N2 = parseInt (EncStr[i+4], 16);
      if (Encrypted)
         N0 = N0 ^ 0x0F;   // Not really encrypted, but obfuscated
      var Code = N3*4096 + N2*256 + N1*16 + N0;
      if (Code > 0)
         Str = Str + String.fromCharCode (Code);
   }
   return Str;
}


function fred_report_html0 ()
{
   var val;

   println("  <h2>Internet Download Manager</h2>");
   var IDM_Root = "\\Software\\DownloadManager";


   if (!IsValid (GetRegistryKeyValue(IDM_Root, "idmvers")))
   {
      println ("<p>No IDM installation found.<p>")
      return;
   }

   Version  = String (GetKeyVal(IDM_Root, "idmvers"));
   Serial   = String (GetKeyVal(IDM_Root, "Serial"));
   TempPath = String (GetKeyVal(IDM_Root, "TempPath"));
   println ("<pre>")
   println ("Installed version: " + Version);
   println ("Serial:            " + Serial);
   println ("Temp path:         " + TempPath);
   println ("</pre>")
   
   // Download folders
   // ----------------
   var IDM_Folders  = IDM_Root + "\\FoldersTree";
   var FoldersNodes = GetRegistryNodes (IDM_Folders);
//   var Folders = []
   if (IsValid (FoldersNodes))
   {
      println ("<h3>IDM default download paths</h3>");
      println ("    <table style=\""+table_style+"\">");
      println ("      <tr>");
      PrintTableHeaderCell ("ID");
      PrintTableHeaderCell ("Name");
      PrintTableHeaderCell ("Mask");
      PrintTableHeaderCell ("Path");
      println("      </tr>");

      for(var i=0; i < FoldersNodes.length; i++)
      {
         var Name = FoldersNodes[i];
         var Key = IDM_Folders + "\\" + Name;
         IDStr = String (GetKeyVal(Key, "ID"));
         Mask  = String (GetKeyVal(Key, "mask"));
         PathW = String (GetKeyVal(Key, "pathW"));

         ID = parseInt (IDStr.slice(2),16),
         Path = ConvString16 (PathW, false);

         println("      </tr>");
         PrintTableDataCell ("left", ID);
         PrintTableDataCell ("left", Name);
         PrintTableDataCell ("left", Mask);
         PrintTableDataCell ("left", Path);
         println("      </tr>");

//         Folders[IndexStr] = Name;
      }
      println("    </table>");
   }
   else
   {
      println ("<p>No folders found.</p>")
   }


   // Downloaded files
   // ----------------
   var RootNodes = GetRegistryNodes (IDM_Root);
   if (IsValid(RootNodes))
   {
      println ("<h3>IDM file downloads</h3>");
      println("    <table style=\""+table_style+"\">");
      println("      <tr>");
      PrintTableHeaderCell("Ref");
      PrintTableHeaderCell("File Name");
      PrintTableHeaderCell("Size");
      PrintTableHeaderCell("Size Human");
      PrintTableHeaderCell("Date added");
      PrintTableHeaderCell("Last try date");
      PrintTableHeaderCell("Status");
      PrintTableHeaderCell("Save To");
      PrintTableHeaderCell("Description");
      PrintTableHeaderCell("URL");
      PrintTableHeaderCell("Referer");
      PrintTableHeaderCell("Password");
      PrintTableHeaderCell("U-User");
      PrintTableHeaderCell("U-Password");
      println("      </tr>");

      RootNodes.sort(function(a, b) { return parseInt(a,10) -parseInt(b,10) })
      for(var i=0; i<RootNodes.length; i++)
      {
         if (!isNaN (RootNodes[i]))
         {
            Key = IDM_Root + "\\" + RootNodes[i];  
            var Filename      = GetKeyVal(Key,"Filename");
            var SavedToEnc    = GetKeyVal(Key,"EncLNFSW");
            var Renamed       = GetKeyVal(Key,"FR_FNCD");
            var StrSize       = GetKeyVal(Key,"FileSize");
            var DateAdded     = GetKeyVal(Key,"DateAdded");
            var DateLastTry   = GetKeyVal(Key,"LastTryDate");
            var Status        = GetKeyVal(Key,"Status");
            var Category      = GetKeyVal(Key,"CategoryID");
            var Description   = GetKeyVal(Key,"Description");
            var URL           = GetKeyVal(Key,"Url0");
            var Referer       = GetKeyVal(Key,"Referer");
            var PasswordEnc   = GetKeyVal(Key,"EncPassword");
            var U_User        = GetKeyVal(Key,"U0_u");
            var U_PasswordEnc = GetKeyVal(Key,"U0_EncP");

            var PosQ = Filename.indexOf("?");
            if (PosQ >= 0)
               Filename    = Filename.slice(0, PosQ); // Cut of Filename at first occurence of "?"
            Size        = StrSizeToNum (StrSize);
            SizeHuman   = SizeToHuman  (Size);
            DateAdded   = ConvertSillyDateFormat (DateAdded);
            DateLastTry = ConvertSillyDateFormat (DateLastTry);

            switch (Status)
            {
               case "0x00000000": StatusStr = "Not found"; break;
               case "0x00000003": StatusStr = "Completed"; break;
               default          : StatusStr = Status;
            }
//            SavedTo = Folders[Category];

            SavedTo    = ConvString16 (SavedToEnc   , true);
            Password   = ConvString8  (PasswordEnc  , true);
            U_Password = ConvString8  (U_PasswordEnc, true);

            println("      <tr>");
            PrintTableDataCell ("left", RootNodes[i]);
            if (IsValid (GetRegistryKeyValue(Key, "FR_FNCD")))
                 PrintTableDataCell ("left", Renamed);
            else PrintTableDataCell ("left", Filename);
            PrintTableDataCell ("right", Size);
            PrintTableDataCell ("right", SizeHuman);
            PrintTableDataCell ("left", DateAdded);
            PrintTableDataCell ("left", DateLastTry);
            PrintTableDataCell ("left", StatusStr);
            PrintTableDataCell ("left", SavedTo);
            PrintTableDataCell ("left", Description);
            PrintTableDataCell ("left", URL);
            PrintTableDataCell ("left", Referer);
            PrintTableDataCell ("left", Password);
            PrintTableDataCell ("left", U_User);
            PrintTableDataCell ("left", U_Password);
            println("      </tr>");
         }
      }
      println("    </table>");
      println ("<p>The results above have been verified with IDM 6.26 under Windows 8.1 pro.</p>");
      println ("<p>Not tested yet: Download paths with characters above ASCII range.</p>");
   }
   else
   {
      println ("<p>No file download entries found.</p>")
   }
}

function fred_report_html()
{
   try
   {
      fred_report_html0()
   }

   catch (Exc)
   {
      println ("Exception occured: " + Exc)
   }
}

