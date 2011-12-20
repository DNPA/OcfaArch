<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/ocfadump">
    <dreport>
      <xsl:for-each select="modlist/module">
        <xsl:variable name="module" select="text()"/>
        <xsl:variable name="nodes" select='//fil[@mod=$module]'/>
        <xsl:element name="mdata">
          <xsl:attribute name="module">
            <xsl:value-of select="$module"/>
          </xsl:attribute>
          <xsl:attribute name="count">
               <xsl:value-of select='count($nodes)'/>
          </xsl:attribute>
          <xsl:value-of select='sum($nodes/@size)'/>
        </xsl:element>
      </xsl:for-each>
    </dreport>     
  </xsl:template>
</xsl:stylesheet>
