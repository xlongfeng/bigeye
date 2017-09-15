#!/usr/bin/env python
# -*- coding: utf-8 -*-


#############################################################################
#
# Bigeye - Accessorial Tool for Daily Test
# Copyright (C) 2017 xlongfeng <xlongfeng@126.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################


from sqlalchemy import Column, ForeignKey, \
     Integer, Boolean, String, DateTime, \
     create_engine, desc
from sqlalchemy.orm import relationship, sessionmaker
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.ext.automap import automap_base


Base = declarative_base()


class TestCase(Base):
    __tablename__ = 'test_case'
    
    id = Column(Integer, primary_key=True)
    name = Column(String)
    timestamp = Column(DateTime)


class KeyEvent(Base):
    __tablename__ = 'key_event'
    
    id = Column(Integer, primary_key=True)
    name = Column(String)
    code = Column(Integer)
    down = Column(Boolean)
    timestamp = Column(Integer)
    test_case_id = Column(Integer, ForeignKey('test_case.id'))
    
    test_case = relationship("TestCase", back_populates="key_event_list")

TestCase.key_event_list = relationship("KeyEvent", order_by=KeyEvent.timestamp, back_populates="test_case")

engine = create_engine('sqlite:///storage.sqlite')
Base.metadata.create_all(engine)

session = sessionmaker(engine)()