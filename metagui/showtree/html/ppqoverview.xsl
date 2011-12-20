<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" id="ocfa" version="1.0">

  <xsl:template match="ppqset">
  
    <HTML>
      <HEAD>
        <meta http-equiv="Refresh" content="4" />
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
            <xsl:for-each select="error">
                <font color="red"><h3><b>ERROR:</b><xsl:value-of select="@msg" /></h3></font>
            </xsl:for-each>

            <h2>PPQ Overview</h2>

	    <xsl:choose>
		<xsl:when test="diskavailable">
		   <p><b>INFO:</b> Available diskspace = <xsl:value-of select="diskavailable/text()" /></p>
		</xsl:when>
	    </xsl:choose>
            
            <table>
              <tr>
                <TH>Module</TH>
                <TH>prio<br/> 0</TH>
                <TH>prio<br/> 1</TH>
                <TH>prio<br/> 2</TH>
                <TH>prio<br/> 3</TH>
                <TH>prio<br/> 4</TH>
                <TH>prio<br/> 5</TH>
                <TH>prio<br/> 6</TH>
                <TH>never<br/> 7</TH>
              </tr>
                <xsl:for-each select="ppq"><tr>
                  <td><xsl:value-of select="@module"/></td>
                  <td>
                    <xsl:choose>
                      <xsl:when test="fifo[@prio='0']/@used = '1'">
                        <a><xsl:attribute name="href"><xsl:text>ppqcontent/</xsl:text><xsl:value-of select="@module"/>?0</xsl:attribute><xsl:value-of select="fifo[@prio='0']/text()"/></a>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="fifo[@prio='0']/text()"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </td>
                  <td>
                    <xsl:choose>
                      <xsl:when test="fifo[@prio='1']/@used = '1'">
                        <a><xsl:attribute name="href"><xsl:text>ppqcontent/</xsl:text><xsl:value-of select="@module"/>?1</xsl:attribute><xsl:value-of select="fifo[@prio='1']/text()"/></a>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="fifo[@prio='1']/text()"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </td>
                  <td>
                    <xsl:choose>
                      <xsl:when test="fifo[@prio='2']/@used = '1'">
                        <a><xsl:attribute name="href"><xsl:text>ppqcontent/</xsl:text><xsl:value-of select="@module"/>?2</xsl:attribute><xsl:value-of select="fifo[@prio='2']/text()"/></a>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="fifo[@prio='2']/text()"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </td>
                  <td>
                    <xsl:choose>
                      <xsl:when test="fifo[@prio='3']/@used = '1'">
                        <a><xsl:attribute name="href"><xsl:text>ppqcontent/</xsl:text><xsl:value-of select="@module"/>?3</xsl:attribute><xsl:value-of select="fifo[@prio='3']/text()"/></a>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="fifo[@prio='3']/text()"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </td>
                  <td>
                    <xsl:choose>
                      <xsl:when test="fifo[@prio='4']/@used = '1'">
                        <a><xsl:attribute name="href"><xsl:text>ppqcontent/</xsl:text><xsl:value-of select="@module"/>?4</xsl:attribute><xsl:value-of select="fifo[@prio='4']/text()"/></a>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="fifo[@prio='4']/text()"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </td>
                  <td>
                    <xsl:choose>
                      <xsl:when test="fifo[@prio='5']/@used = '1'">
                        <a><xsl:attribute name="href"><xsl:text>ppqcontent/</xsl:text><xsl:value-of select="@module"/>?5</xsl:attribute><xsl:value-of select="fifo[@prio='5']/text()"/></a>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="fifo[@prio='5']/text()"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </td>
                  <td>
                    <xsl:choose>
                      <xsl:when test="fifo[@prio='6']/@used = '1'">
                        <a><xsl:attribute name="href"><xsl:text>ppqcontent/</xsl:text><xsl:value-of select="@module"/>?6</xsl:attribute><xsl:value-of select="fifo[@prio='6']/text()"/></a>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="fifo[@prio='6']/text()"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </td>
                  <td>
                    <xsl:choose>
                      <xsl:when test="fifo[@prio='7']/@used = '1'">
                        <a><xsl:attribute name="href"><xsl:text>ppqcontent/</xsl:text><xsl:value-of select="@module"/>?7</xsl:attribute><xsl:value-of select="fifo[@prio='7']/text()"/></a>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="fifo[@prio='7']/text()"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </td>
                </tr></xsl:for-each>
            </table>
            </div> 
         </div>
      </BODY>
    </HTML>
  
  </xsl:template> <!-- End template case -->



      


</xsl:stylesheet>
