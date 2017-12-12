(function(){
    
    //connect with postgresql
    
    const pg = require('pg');
    const connectionString = process.env.DATABASE_URL || 'postgres://postgres:123@localhost:5432/websql';
    const client = new pg.Client(connectionString);
    client.connect();
    
    //send query to database
    function doQuery(queryToDb) {
        return client.query(queryToDb, (err, res) => {
            if (err) throw err
            client.end()
            console.log("query enviada!");
        })
    }

    module.exports = {
        doQuery: doQuery
    }

})()