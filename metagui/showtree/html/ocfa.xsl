<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" id="ocfa" version="1.0">

  <xsl:template match="evidence">
  
    <!-- Fill var href_up -->
    <xsl:variable name="href_up">
        <xsl:choose>
	  <xsl:when test="/evidence/@id=0">
	     <xsl:text>showtree.cgi?case=</xsl:text><xsl:value-of select="@case" />
	  </xsl:when>
	  <xsl:otherwise>
	     <xsl:text>evidence.cgi?case=</xsl:text><xsl:value-of  select="@case"/>&amp;src=<xsl:value-of select="@src"/>&amp;item=<xsl:value-of select="@item"/>&amp;child=<xsl:value-of select="@id"/>
	  </xsl:otherwise>
	</xsl:choose>
    </xsl:variable>
  
  
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
               <a href="/index.cgi">HOME</a>
               <a href="lucene.cgi">INDEX</a>
               <a href="/cgi-bin/meta.cgi">Overview</a>
               <a href="/cgi-bin/ppqoverview">PPQ</a>

              </div>
            </div><!-- end div.head -->

		<div class="case_admin">
			<table>
			  <tr>
				<td width="80px">Case:</td>
				<td width="135px"><xsl:value-of  select="@case"/></td>
			  </tr>
                          <tr>
                                <td>Source:</td>
                                <td><xsl:value-of select="@src"/></td>
                          </tr>
                          <tr>
                                <td>Item:</td>
                                <td><xsl:value-of select="@item"/></td>
                          </tr>
			</table>
		</div>
            <div class="children">

            
     <xsl:choose>
       <xsl:when test="//job/moduleinstance/@module='dsm'">
            <h3>Evidence browser</h3>
          </xsl:when>
	  <xsl:when test="//job/moduleinstance/@module='DATASTORE'">
            <h3>Evidence browser</h3>
	  </xsl:when>
	  <xsl:otherwise>
	    <h3 class="incomplete">!! INCOMPLETE RUN !!</h3>
        </xsl:otherwise>
     </xsl:choose>            
             
     <!-- Table to browse evidence -->
      <table>
        <tr>
          <th width="140px">
	    <a><xsl:attribute name="href"><xsl:value-of select="$href_up" /></xsl:attribute>
		<img src="/icons/back.gif" alt="up" border="0px" />
	    </a>
	  </th>
          <th width="500px" colspan="2"><xsl:value-of  select="@case"/> :: <xsl:value-of select="@src"/> :: <xsl:value-of select="@item"/> :: <a><xsl:attribute name="href">
		<xsl:value-of select="$href_up" /></xsl:attribute>
		<xsl:value-of select="/evidence/location/text()"/></a><xsl:text> / </xsl:text><xsl:value-of select="/evidence/location/@name"/>
	  </th>
          <th></th>
        </tr>

        <!-- loop throught all childevidences -->
	<xsl:for-each select="/evidence/job/childevidence">
	  <xsl:sort select="@relname" />
	  <xsl:sort select="@name" />
          <tr>
            <td>  
              <xsl:choose>
		<xsl:when test="@relname='dirdirentry'">
		    <img src="/icons/dir.gif" alt="D"/>
		</xsl:when>
                <xsl:when test="@relname='filedirentry'">
                    <img src="/icons/generic.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='specialdirentry'">
                    <img src="/icons/generic.red.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='removedentry'">
                    <img src="/icons/burst.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='partitionentry'">
                    <img src="/icons/pie1.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='swappartitionentry'">
                    <img src="/icons/pie6.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='unallocated'">
                    <img src="/icons/pie0.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='slack'">
                    <img src="/icons/continued.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='fsinfo'">
                    <img src="/icons/screw2.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='content'">
                    <img src="/icons/folder.sec.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='headers'">
                    <img src="/icons/transfer.gif" alt="D"/>
                </xsl:when>
                <xsl:when test="@relname='undefined'">
                    <img src="/icons/transfer.gif" alt="D"/>
                </xsl:when>
		<xsl:otherwise><img src="/icons/broken.gif" alt="F"/></xsl:otherwise>
	     </xsl:choose>
           </td>
           <td>
	     <a><xsl:attribute name="href">evidence.cgi?case=<xsl:value-of select="/evidence/@case"/>&amp;src=<xsl:value-of select="/evidence/@src"/>&amp;item=<xsl:value-of select="/evidence/@item"/>&amp;id=<xsl:value-of select="@evidenceid"/></xsl:attribute>
		<xsl:value-of select="@name"/></a>
           </td>
           <td>
              <xsl:value-of select="../moduleinstance/@module"/>
           </td>
          </tr>
	</xsl:for-each>
      </table>            
            
            </div><!-- end div.children -->

  
            <div class="meta">
               <h2>Detailed meta information for evidence:</h2>
                <xsl:apply-templates select="location" />
            </div><!-- end div.meta -->
            
         </div><!-- end div.wrapper -->
      </BODY>
    </HTML>      
  </xsl:template>  <!-- Match evidence -->


 <!-- 
    **** Template for Location tag **********************************************
    --> 
  <xsl:template match="location">

      <table>
	<tr>
	   <th width="140px">
             <xsl:choose> <!-- ICON -->
				<xsl:when test="//meta[@name='mimetop']/scalar/text()='text'">
				    <img src="/icons/text.gif" alt="text"/>
				</xsl:when>
				<xsl:when test="//meta[@name='mimetop']/scalar/text()='video'">
				    <img src="/icons/movie.gif" alt="video"/>
				</xsl:when>
				<xsl:when test="//meta[@name='mimetop']/scalar/text()='audio'">
				    <img src="/icons/sound2.gif" alt="audio"/>
				</xsl:when>
				<xsl:when test="//meta[@name='mimetop']/scalar/text()='image'">
				    <img src="/icons/image2.gif" alt="image"/>
				</xsl:when>
				<xsl:when test="//meta[@name='mimetop']/scalar/text()='application'">
				    <img src="/icons/binary.gif" alt="application"/>
				</xsl:when>
				<xsl:when test="//meta[@name='inodetype']/scalar/text()='dir'">
				    <img src="/icons/dir.gif" alt="D" />
				</xsl:when>
				<xsl:when test="//meta[@name='digestsource']/scalar/text()='NIST'">
				    <img src="/icons/nist.gif" alt="NIST" />
				</xsl:when>
				<xsl:when test="//meta[@name='digestsource']/scalar/text()='KP'">
				    <img src="/icons/kp.gif" alt="ChildPorn!" />
				</xsl:when>
				<xsl:otherwise>
				    <img src="/icons/unknown.gif" alt="unknown"/>
 				</xsl:otherwise>
 	     </xsl:choose>
 	   </th>
	   <th width="300px"><xsl:value-of select="/evidence/location/@name"/></th>
	   <th width="200px">
	      <xsl:if test="/evidence/@storeref">   
              <!-- <xsl:if test="//meta[@name='inodetype']/scalar/text()='file'"> -->
                 <a><xsl:attribute name="href">view.cgi?case=<xsl:value-of select="/evidence/@case"/>&amp;src=<xsl:value-of select="/evidence/@src"/>&amp;item=<xsl:value-of select="/evidence/@item"/>&amp;id=<xsl:value-of select="/evidence/@id"/>&amp;mime=<xsl:value-of select="//meta[@name='mimetype']/scalar[1]/text()"/></xsl:attribute><xsl:text>View content</xsl:text></a>
              </xsl:if>	   
	   
	   </th>
	</tr>
      <tr>
	   <td>Job</td>
	   <td>MetaName</td>
	   <td>MetaValue</td>
      </tr>

      <!-- Iterate over all meta's -->
      <xsl:for-each select="/evidence/job">
         <TR>
            <TH><xsl:value-of select="moduleinstance/@module"/></TH>
            <TH>stime = <xsl:value-of select="@stime"/></TH>
            <TH>etime = <xsl:value-of select="@etime"/></TH>
         </TR>
         <xsl:for-each select="meta">
	       <xsl:choose>
		 <xsl:when test="@type='scalar'">
		    <tr>
                      <TD></TD>
                      <TD><xsl:value-of select="@name"/></TD>
                      <TD><xsl:value-of select="scalar/text()"/></TD>
		    </tr>
		 </xsl:when>
		 <xsl:when test="@type='table'">
		    <xsl:for-each select="head">
		      <tr>
                        <TD></TD>
                        <TD>Tablehead</TD>
                        <TD><xsl:value-of select="text()"/></TD>
		      </tr>
		    </xsl:for-each>
		    <xsl:for-each select="scalar">
		      <tr>
                   	<TD></TD>
                   	<TD><xsl:value-of select="@type"/></TD>
                   	<TD><xsl:value-of select="text()"/></TD>
		      </tr>
		    </xsl:for-each>
		    

		 </xsl:when>
	       </xsl:choose>
	
         </xsl:for-each>
      </xsl:for-each>

	
      </table>

  </xsl:template> <!-- End template location -->



      


</xsl:stylesheet>
