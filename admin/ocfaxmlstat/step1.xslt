<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="//evidence">
     <xsl:element name="fil">
       <xsl:attribute name="md5">                        
         <xsl:value-of select="@md5"/>
       </xsl:attribute>
       <xsl:attribute name="mod">                        
         <xsl:value-of select="job[1]/moduleinstance/@module"/>
       </xsl:attribute>
       <xsl:for-each select="job[1]/meta[@name='size']">
          <xsl:attribute name="size">
             <xsl:value-of select="scalar"/>
          </xsl:attribute>
       </xsl:for-each>
     </xsl:element>
  </xsl:template>
</xsl:stylesheet>
