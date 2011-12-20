<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" id="ocfa" version="1.0">

  <xsl:template match="error">
  
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
            <h2>ERROR</h2>
            <h3>The following error has occurred:</h3>
            <table>
              <tr>
                <TH width="100px">Error</TH>
                <TH width="500px"><xsl:value-of select="/error/@msg"/></TH>
              </tr>
              <xsl:for-each select="note">
                <tr>
                  <td>Note</td>
                  <td><xsl:value-of select="@msg"/></td>
                </tr>
              </xsl:for-each>
            </table>
            </div> 
         </div>
      </BODY>
    </HTML>
  
  </xsl:template> <!-- End template case -->



      


</xsl:stylesheet>
