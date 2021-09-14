INSERT INTO `idle_locked` (`id`)
  SELECT `idle_store`.`id` FROM `idle_store`
    LEFT OUTER JOIN `idle_locked` ON `idle_store`.`id` = `idle_locked`.`id`
  WHERE `idle_locked`.`id` IS NULL
    AND `idle_store`.`key` = 1
  ORDER BY `idle_store`.`id` ASC LIMIT 1;

-- SELECT `data` FROM `idle_store` WHERE `id` = ?;

-- SELECT last_insert_rowid();

/*
SELECT *
FROM `idle_store`
INNER JOIN `idle_keys` ON `idle_keys`.`key` = `idle_store`.`key`
WHERE `name` = 'idle::myservice';
*/
