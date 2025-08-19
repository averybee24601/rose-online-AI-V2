-- Item Mall custom additions: costumes, hats, wings, carts, engines, decorations, mounts
-- Safe to re-run: delete target slots before insert

-- Costumes: Sets (Category 1, SubCategory 0)
DELETE FROM itemmall WHERE category=1 AND subcategory=0 AND slot IN (0,1,2,3,4,5,6,7,8);
INSERT INTO itemmall (name,itemid,itemtype,price,amount,category,subcategory,slot,cmdl2,cmdl1,icon,active,promotion) VALUES
('Boxing Look',1211,10,1200,1,1,0,0,'','',1,1,0),
('Servants Look',1212,10,1200,1,1,0,1,'','',1,1,0),
('Navy Blue Magic Set',1217,10,1500,1,1,0,2,'2% EXP Bonus!','',1,1,0),
('White Magic Set',1218,10,1500,1,1,0,3,'2% EXP Bonus!','',1,1,0),
('New Abyss Set',1216,10,1600,1,1,0,4,'120 Soldier 2nd Job','',2,1,0),
('New Captain Set',1214,10,1600,1,1,0,5,'120 Hawker 2nd Job','',2,1,0),
('New Bourgeois Set',1213,10,1600,1,1,0,6,'120 Dealer 2nd Job','',2,1,0),
('New Serenity Set',1215,10,1600,1,1,0,7,'120 Muse 2nd Job','',2,1,0),
('Eastern Elegance Set',1237,10,1500,1,1,0,8,'','',1,1,0);

-- Costumes: Hats (Category 1, SubCategory 1)
DELETE FROM itemmall WHERE category=1 AND subcategory=1 AND slot IN (0,1,9,10,11,12,13,14,15,16,17,18);
INSERT INTO itemmall (name,itemid,itemtype,price,amount,category,subcategory,slot,cmdl2,cmdl1,icon,active,promotion) VALUES
('Ancient Headdress',223,2,700,1,1,1,0,'','',1,1,0),
('Halo',299,2,700,1,1,1,1,'','',1,1,0),
('Elf Ears',4001,2,700,1,1,1,9,'','Account Bound',1,1,0),
('Devil Horn',815,2,700,1,1,1,10,'','',1,1,0),
('Dread Hat',166,2,700,1,1,1,11,'','',1,1,0),
('Devourer Hat',167,2,700,1,1,1,12,'','',1,1,0),
('Cat Hat',804,2,700,1,1,1,13,'','',1,1,0),
('Black Cat Hat',181,2,700,1,1,1,14,'','',1,1,0),
('Katyusha Kitty Ears',838,2,700,1,1,1,15,'','',1,1,0),
('KeroKero Hat',826,2,700,1,1,1,16,'','',1,1,0),
('Students Cap',834,2,700,1,1,1,17,'','',1,1,0),
('Angel Hat',825,2,700,1,1,1,18,'','',1,1,0);

-- Costumes: Backs / Wings / Shields (Category 1, SubCategory 4)
DELETE FROM itemmall WHERE category=1 AND subcategory=4 AND slot IN (0,1,2,3,4,5,6,7,8,9,10,11);
INSERT INTO itemmall (name,itemid,itemtype,price,amount,category,subcategory,slot,cmdl2,cmdl1,icon,active,promotion) VALUES
('Astarot Black Wings',771,6,1800,1,1,4,0,'','',2,1,0),
('Astarot White Wing',763,6,1800,1,1,4,1,'','',2,1,0),
('Astarot Blue Wings',774,6,1800,1,1,4,2,'','',2,1,0),
('Astarot Red Wings',772,6,1800,1,1,4,3,'','',2,1,0),
('Astarot Pink Wing',764,6,1800,1,1,4,4,'','',2,1,0),
('Astarot Purple Wings',773,6,1800,1,1,4,5,'','',2,1,0),
('Astarot Green Wing',765,6,1800,1,1,4,6,'','',2,1,0),
('Astarot Silver Wing',762,6,1800,1,1,4,7,'','',2,1,0),
('Seraph Wing',1236,6,1900,1,1,4,8,'','',2,1,0),
('Black Seraph Wing',1235,6,1900,1,1,4,9,'','',2,1,0),
('Cross Shield',753,6,1500,1,1,4,10,'','',1,1,0),
('Black Cross Shield',770,6,1500,1,1,4,11,'','',1,1,0);

-- Carts: Frames (Category 3, SubCategory 0)
DELETE FROM itemmall WHERE category=3 AND subcategory=0 AND slot IN (0,1,2,3,4,5,6,7,8);
INSERT INTO itemmall (name,itemid,itemtype,price,amount,category,subcategory,slot,cmdl2,cmdl1,icon,active,promotion) VALUES
('Meister Flame Frame',429,14,2200,1,3,0,0,'','',1,1,0),
('Meister W Frame',421,14,2200,1,3,0,1,'','',1,1,0),
('Meister B Frame',423,14,2200,1,3,0,2,'','',1,1,0),
('Meister R Frame',422,14,2200,1,3,0,3,'','',1,1,0),
('Meister Y Frame',424,14,2200,1,3,0,4,'','',1,1,0),
('Meister Pink Frame',425,14,2200,1,3,0,5,'','',1,1,0),
('Meister Gray Frame',427,14,2200,1,3,0,6,'','',1,1,0),
('Meister Green Frame',428,14,2200,1,3,0,7,'','',1,1,0),
('Meister Blue Frame',426,14,2200,1,3,0,8,'','',1,1,0);

-- Carts: Engine (Category 3, SubCategory 2)
DELETE FROM itemmall WHERE category=3 AND subcategory=2 AND slot IN (0);
INSERT INTO itemmall (name,itemid,itemtype,price,amount,category,subcategory,slot,cmdl2,cmdl1,icon,active,promotion) VALUES
('Chaser Engine',451,14,1800,1,3,2,0,'','',1,1,0);

-- Carts: Decorations / Boosters (Category 3, SubCategory 3)
DELETE FROM itemmall WHERE category=3 AND subcategory=3 AND slot IN (0,1,3,4,5,6,7,8,9,10);
INSERT INTO itemmall (name,itemid,itemtype,price,amount,category,subcategory,slot,cmdl2,cmdl1,icon,active,promotion) VALUES
('Sabertooth Mount (7 Day)',2,15,1200,1,3,3,0,'New Mount!','',1,1,0),
('Lunar Wolf Mount (7 Day)',3,15,1200,1,3,3,1,'New Mount!','',1,1,0),
('PAT Painting Ticket',442,14,500,1,3,3,3,'Customize PAT items','',1,1,0),
('Spike Wheel',401,14,900,1,3,3,4,'','',1,1,0),
('Speed Wing',402,14,900,1,3,3,5,'','',1,1,0),
('Wind Booster',403,14,900,1,3,3,6,'','',1,1,0),
('Jet Booster',404,14,900,1,3,3,7,'','',1,1,0),
('Pink Moldie Shader',407,14,800,1,3,3,8,'','',1,1,0),
('Blue Moldie Shader',406,14,800,1,3,3,9,'','',1,1,0),
('Colorful Umbrella',405,14,800,1,3,3,10,'','',1,1,0);

-- Notes:
-- 1) These entries rely on matching records in Client ItemMallListings.xml by Category/SubCategory/Slot.
-- 2) Prices are placeholders; adjust to your economy.
-- 3) Ensure server reloads or run this against the live database to take effect.


