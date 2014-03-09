-- 1 page read only because when select COUNT(*) AND key-specified condition 
-- is out of data range, it will read from the first page of xlarge.indx and give
-- the result directly
SELECT COUNT(*) FROM xlarge;
SELECT COUNT(*) FROM xlarge where key<2147483647

-- 29 pages read because it didn't read xlarge.tbl
SELECT COUNT(*) FROM xlarge WHERE key<2500 AND key>1000;
SELECT key FROM xlarge WHERE key<2500 AND key>1000;

-- 1061 pages read because whenever it has a value-specifed condition or
-- select value statement it will use xlarge.idx and also read xlarge.tbl
SELECT * FROM xlarge WHERE key<2500 AND key>1000;
SELECT value FROM xlarge WHERE key<2500 AND key>1000;
SELECT key FROM xlarge WHERE value <= 'Z' AND key<2500 AND key>1000;

-- These three queries are equivalent. The first query will use a linear scan while the second
-- one will do a index-based scan. However, in this case linear scan will be much faster than 
-- index-based scan. The third query is the simplified version from the first and second query
SELECT * FROM xlarge WHERE key>0 AND key < 2016123412;
SELECT * FROM xlarge WHERE key>0 AND key < 2016123410;
SELECT * FROM xlarge WHERE key>0

-- A bug was fixed with this query
SELECT count(*) FROM xlarge WHERE key>100 AND key<>40

-- These two cases have been optimized. When key-specified conditions are out of range, we don't
-- need to scan either index or record file
SELECT * FROM xlarge WHERE key > 2016123412;
SELECT * FROM xlarge WHERE key < 2;

