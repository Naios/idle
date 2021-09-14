

DROP TABLE IF EXISTS `idle_keys`;
DROP TABLE IF EXISTS `idle_store`;
DROP TABLE IF EXISTS `idle_locked`;

PRAGMA foreign_keys = OFF;

CREATE TABLE IF NOT EXISTS `idle_keys` (
  `key` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `name` TEXT UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS `idle_store` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `key` INTEGER NOT NULL,
  `checksum` INTEGER NOT NULL DEFAULT 0,
  `data` BLOB NULL DEFAULT NULL,
  `expiry` DATETIME NULL DEFAULT NULL,
  `temp` BOOLEAN NOT NULL DEFAULT 0 CHECK (`temp` IN (0, 1)),
  FOREIGN KEY (`key`) REFERENCES `idle_keys` (`key`) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS `idle_locked` (
  `id` INTEGER PRIMARY KEY NOT NULL,
  FOREIGN KEY (`id`) REFERENCES `idle_store` (`id`) ON UPDATE CASCADE ON DELETE CASCADE
);

PRAGMA foreign_keys = ON;

-- SELECT FROM `store`

-- last_insert_rowid()
-- VACUUM
/*INSERT INTO `idle_keys` (`name`) VALUES ('idle::myservice') ON CONFLICT(`name`) DO UPDATE SET name = name;
INSERT INTO `idle_keys` (`name`) VALUES ('idle::myservice') ON CONFLICT(`name`) DO UPDATE SET name = name;
INSERT INTO `idle_keys` (`name`) VALUES ('idle::myservice') ON CONFLICT(`name`) DO UPDATE SET name = name;
INSERT INTO `idle_keys` (`name`) VALUES ('idle::myservice') ON CONFLICT(`name`) DO UPDATE SET name = name;
INSERT INTO `idle_keys` (`name`) VALUES ('idle::myservice') ON CONFLICT(`name`) DO UPDATE SET name = name;
INSERT INTO `idle_keys` (`name`) VALUES ('idle::myservice') ON CONFLICT(`name`) DO UPDATE SET name = name;

SELECT last_insert_rowid();

INSERT INTO `idle_store` (`key`, `data`) VALUES (1, 'hu');
INSERT INTO `idle_store` (`key`, `data`) VALUES (1, 'bu');
INSERT INTO `idle_store` (`key`, `data`) VALUES (1, 'tu');
INSERT INTO `idle_store` (`key`) VALUES (1);

SELECT `id`, `data` FROM `idle_store` WHERE `key` = 'idle::myservice' ORDER BY `id` ASC LIMIT 1;
*/
