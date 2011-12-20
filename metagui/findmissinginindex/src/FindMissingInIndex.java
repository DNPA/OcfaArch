import org.apache.lucene.index.*;
import org.apache.lucene.document.*;
import java.sql.*;
import java.util.HashSet;
import java.util.Properties;
import java.io.IOException;
import java.io.FileInputStream;

public class FindMissingInIndex {

    private HashSet<String>indexedDocs;
    private PreparedStatement mFindChildrenStatement;
    public FindMissingInIndex(String inLuceneDir)
	throws IOException {
	
	fillIndexedDocs(inLuceneDir);

    }

    private void fillIndexedDocs(String inLuceneDir) throws IOException {

	IndexReader theReader = IndexReader.open(inLuceneDir);
	int maxDocs =  theReader.numDocs();
	indexedDocs = new HashSet<String>(maxDocs);
	for (int x  = 0; x < maxDocs; x++){

	    Document doc = theReader.document(x);
	    Field dataid = doc.getField("docid");
	    if (dataid != null){
		//System.out.println("String Value is " + dataid.stringValue());
		indexedDocs.add(dataid.stringValue());
	    }
	}
	
    }

    public void findMissingFiles(String inCase, String inSource, String inItem) throws 
	IOException, SQLException, ClassNotFoundException {

	Properties caseProps = new Properties();
	caseProps.load(new FileInputStream("/usr/local/digiwash/etc/" + inCase + ".conf"));
	
	Class.forName("org.postgresql.Driver");
	String url = "jdbc:postgresql://" + caseProps.get("storedbhost") + "/" 
	    + inCase + "?user=ocfa";
	Connection connection = DriverManager.getConnection(url);
	Statement statement = connection.createStatement();
	mFindChildrenStatement = connection.prepareStatement("select count(*) as cnt from MetaDataInfo where evidence like ?"); 
	statement.setFetchSize(100);
	String query = "select evidence, dataid, location from MetaDataInfo "
	    + "inner join item on MetaDataInfo.itemid = Item.itemid "
	    + " where Item.casename = '" + inCase + "' and Item.evidencesource = '" 
	    + inSource + "' and Item.item = '" + inItem + "' and metadatainfo.dataid is not null";
	ResultSet set = statement.executeQuery(query);
	System.out.println("<html><body><h1>Files die niet in de index zitten</h1><table border=\"0\">");
	int x = 0;
	while (set.next()){
	    
	    if( (x % 1000) == 0){
	    
		System.err.println("checks " + x);
	    }
	    String evidenceString = set.getString("evidence");
	    String dataid = set.getString("dataid");
	    if (isMissing(evidenceString, dataid)){

		String link = "/cgi-bin/evidence.cgi?case=" + inCase + "&src=" + inSource
		    + "&item=" + inItem + "&id=" + evidenceString; 
		System.out.println("<tr><td>");
		System.out.println("<a href=\"" + link + "\"" + set.getString("location") + "</a></td></tr>");
	    }
	    x++;
	}
	System.out.println("</table>");
    }

    protected boolean isMissing(String inEvidenceString, String inDataId) throws SQLException { 

	return !(indexedDocs.contains(inDataId) || hasChildren(inEvidenceString));
    }

    protected boolean hasChildren(String inEvidenceString)throws SQLException{

	mFindChildrenStatement.setString(1, inEvidenceString + ".%");
	ResultSet result = mFindChildrenStatement.executeQuery();
	result.next();
	return (result.getInt("cnt") > 0);
    }

    static public  void main(String argv[]){

	if (argv.length < 2){

	    System.out.println("usages FindMissingIndex <case> <indexdir> <src> <item>");
	}
	else {
	    try {
		FindMissingInIndex missing = new FindMissingInIndex(argv[1]);
		System.err.println("Starting the check");
		missing.findMissingFiles(argv[0], argv[2], argv[3]);
	    } catch(IOException e){

		e.printStackTrace();
	    } catch(SQLException e){

		e.printStackTrace();
	    }catch (ClassNotFoundException e){

		e.printStackTrace();
	    }
	}	
    }
}
