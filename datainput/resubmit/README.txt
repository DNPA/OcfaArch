Thge resubmitter is a simple tool that sends already submitted evidence to the router 
with newly added metadata. It can be used when a set of files has taken a wrong route through 
ocfa.

Syntax is:

resubmit <file-with-metadataids> [<metaname> <value>]+

The file with metadataids should consist of the metadataids of the to-be-resubmitted evidences seperated by a newline.

e.g.:

135
2154
3543

The resubmitter creates a new job on the evidence that already went to the dsm and sends it to the router.
The metaname name/value pairs are pieces of metadata (string values) that are added.

Some caveats:

1) normally ocfa gives an error when an evidence goes through a module twice (with some exceptions) you can 
(and should) change this behavior by checking the moduletwice parameter in your <case>.conf file. Options are fail or ok.

2) The resubmitter is very slow on large cases. This is caused because it normally removes all metadata from the tables in 
the database. The rationale here is that you do not want the dsm have to enter your data twice in the different row-<meta> tables.

This behvior is currently not configurable, but you can easily change it by commenting out the code. You'll then have to make appriopriate changes to 
the dsm.conf too to make sure that that stuff like rowmimetype is not stored twice.

