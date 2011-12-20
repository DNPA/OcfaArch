<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" id="ocfa" version="1.0">

  <xsl:template match="case">
  
   <HTML>
      <HEAD>
        <TITLE>Open Computer Forensics Architecture</TITLE>
        <link rel="stylesheet" type="text/css" href="/ocfa.css" />
      </HEAD>
      
      <BODY>
         <div class="wrapper">
            <div class="head">
              <h1>Open Computer Forensics Architecture</h1>
              <div class="nav">
                <a href="/index.cgi">Home</a>
                <a href="/cgi-bin/lucene.cgi">Index</a>
                <a href="/cgi-bin/meta.cgi">Overview</a>
                <a href="/cgi-bin/ppqoverview">PPQ</a>
              </div>
            </div><!-- end div.head -->
            <div class="children">
            <h2>CASE: <xsl:value-of select="@name"/></h2>
            <p>The table below shows all source and items of the case <b><i><xsl:value-of select="@name"/></i></b>.<br/> 
            Please select a row to browse the metadata.</p>
            <table>
              <tr>
                <TH width="200px">Source</TH>
                <TH width="200px">Item</TH>
                <TH width="60px">Link</TH>
              </tr>
            <xsl:for-each select="item">
              <TR>
                <TD><xsl:value-of select="@src"/></TD>
                <TD><xsl:value-of select="@name"/></TD>
                <TD>
                  <a>
                    <xsl:attribute name="href"><xsl:text>/cgi-bin/evidence.cgi?case=</xsl:text><xsl:value-of  select="/case/@name"/>&amp;src=<xsl:value-of select="@src"/>&amp;item=<xsl:value-of select="@name"/>&amp;id=0</xsl:attribute>Browse
                  </a>
                </TD>
              </TR>
            </xsl:for-each>
            </table>
            
            </div> <!-- childeren -->
         </div> <!-- wrapper -->
      </BODY>
   </HTML>  
  
  
 </xsl:template> <!-- End template case -->



      


</xsl:stylesheet>