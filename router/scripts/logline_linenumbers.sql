ALTER TABLE loglines ADD ruleline int;

UPDATE loglines 
SET ruleline = int4(regexp_replace(regexp_replace(line,'.*line=',''),':.*$','')) ;
