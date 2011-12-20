SELECT line, rule from rulelist where line not in
(
	SELECT distinct ruleline 
	from loglines 
	where ruleline > 0 AND
	metadataid in (select metadataid from metadatainfo where itemid in (5))
)
