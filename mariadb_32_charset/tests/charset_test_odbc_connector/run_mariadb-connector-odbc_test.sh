#!/bin/bash

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2021 Red Hat, Inc.
#
#   This copyrighted material is made available to anyone wishing
#   to use, modify, copy, or redistribute it subject to the terms
#   and conditions of the GNU General Public License version 2.
#
#   This program is distributed in the hope that it will be
#   useful, but WITHOUT ANY WARRANTY; without even the implied
#   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#   PURPOSE. See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free
#   Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#   Boston, MA 02110-1301, USA.
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

. /usr/share/beakerlib/beakerlib.sh || exit 1

INPUT_DATABASES="${DATABASES:-mariadb}"

rlJournalStart
    rlPhaseStartSetup
        rlRun "TmpDir=\$(mktemp -d)" 0 "Creating tmp directory"
        rlRun "cp base_test.c Makefile odbc.ini odbc_driver.ini $TmpDir"
        rlRun "pushd $TmpDir"
        rlRun "rlImport 'database/mariadb'"
        export DATABASE=mariadb
        rlFileBackup --clean "${DATABASE}DbDir"
    rlPhaseEnd

    rlPhaseStartTest "$DATABASE"
        rlRun "${DATABASE}Start"
        rlRun "sleep 10" 0 "Wait till the systemd service is fully started"
        rlRun "mysql -u root <<< \"CREATE DATABASE dbtest\"" 0 "Creating database dbtest"
        rlRun "${DATABASE}AddUser dbtester dbtester" \
                0 "Creating database user dbtester"
        rlRun "mysql -u root <<< \"GRANT ALL on dbtest.* TO 'dbtester'@'localhost';\"" 0 "Granting privileges on dbtest for dbtester"
        rlRun "mysql -u root <<< \"CREATE TABLE dbtest.tabletest(Id VARCHAR(25), Forename VARCHAR(40), Surename VARCHAR(40));\"" 0 "Create a table tabletest for example data"

        rlRun "odbcinst -i -s -l -f odbc.ini" 0 "Installing DSN"
        rlRun "odbcinst -i -d -f odbc_driver.ini" 0 "Installing ODBC driver"
        rlRun -l -t "isql MariaDB-server <<< \"INSERT INTO tabletest VALUES ('1','Čeněk Řehák','z Žabovřesek');\" 2>&1| grep ERROR " 1 "Writing to the test table"
        rlRun -l -t "isql MariaDB-server <<< \"INSERT INTO tabletest VALUES ('2','Cenek Rehak','z Zabovresek');\" 2>&1| grep ERROR " 1 "Writing to the test table"
        rlRun -l -t "isql MariaDB-server <<< \"SELECT * FROM tabletest\"" 0 "Reading from the test table."

        rlRun "mysql -u root <<< \"DROP TABLE dbtest.tabletest\"" 0 "Dropping database table"
        rlRun "mysql -u root <<< \"DROP DATABASE dbtest\"" 0 "Dropping database dbtest"
        rlRun "mysql -u root <<< \"DROP USER 'dbtester'@'localhost'\"" 0 "Dropping DB user dbtester"
        rlRun "${DATABASE}Stop"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rlFileRestore"
        rlRun "popd"
        rlRun "rm -r $TmpDir" 0 "Removing tmp directory"
    rlPhaseEnd
rlJournalEnd
rlJournalPrintText
