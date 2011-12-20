<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" id="ocfa" version="1.0">

  <xsl:template match="fifo">
  
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
            <table>
              <TR>
                <TH width="150">Case</TH>
                <TH width="150">Module</TH>
                <TH width="70">Priority</TH>
              </TR>
              <TR>
                <TD><xsl:value-of select="@case"/></TD>
                <TD><xsl:value-of select="@module"/></TD>
                <TD><xsl:value-of select="@prio"/></TD>
              </TR>
            </table>
            
            </div> <!-- content -->
            <div class="meta">
              <h3>Queue Content</h3>
              <xsl:if test="//history">
                 <p>No active messages, showing <b><u><xsl:value-of select="//history/@type"/></u></b> history.</p>
              </xsl:if>
              <table>
                <TR>
                  <TH width="150">Sender</TH>
                  <TH width="150">Type</TH>
                  <TH width="150">Subject</TH>
                  <TH width="70">Content</TH>
                </TR>
                <xsl:for-each select="message">
                   <TR>
                     <TD><xsl:value-of select="@sender"/></TD>
                     <TD><xsl:value-of select="@type"/></TD>
                     <TD><xsl:value-of select="@subject"/></TD>
                     <TD>
                     <xsl:choose>
                         <xsl:when test="@type='evidence'">
                            <A>
                               <xsl:attribute name="href">/cgi-bin/evidenceid.cgi?metaid=<xsl:value-of select="text()"/></xsl:attribute>
                               <xsl:value-of select="text()"/>
                           </A>
                         </xsl:when>
                         <xsl:otherwise>
                             <xsl:value-of select="text()"/>
                         </xsl:otherwise>
                     </xsl:choose>
                     </TD>
                   </TR>
                </xsl:for-each>
              </table>
              <xsl:if test="//warning">
                 <h3>Warning:</h3>
                 <p><xsl:value-of select="//warning/text()"/></p>
              </xsl:if>
            </div> <!-- meta -->
         </div>
      </BODY>
    </HTML>
  
  </xsl:template> <!-- End template case -->



      


</xsl:stylesheet>
