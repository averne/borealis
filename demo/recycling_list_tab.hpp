/*
    Copyright 2020-2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include <borealis.hpp>

class RecyclerCell
    : public brls::RecyclerCell
{
  public:
    RecyclerCell();

    BRLS_BIND(brls::Rectangle, accent, "brls/sidebar/item_accent");
    BRLS_BIND(brls::Label, label, "brls/sidebar/item_label");

    static RecyclerCell* create();
};

class DataSource
    : public brls::RecyclerDataSource
{
  public:
    int numberOfRows() override;
    brls::RecyclerCell* cellForRow(brls::RecyclerFrame* recycler, int row) override;
};

class RecyclingListTab : public brls::Box
{
  public:
    RecyclingListTab();

    static brls::View* create();

  private:
    BRLS_BIND(brls::RecyclerFrame, recycler, "recycler");
};
